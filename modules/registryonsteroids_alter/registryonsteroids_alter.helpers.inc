<?php

/**
 * @file
 * registryonsteroids_alter.helpers.inc
 */

/**
 * Extend the #theme_wrappers property of an element.
 *
 * @param array $element
 *   The element.
 * @param string[] $suggestions_parts
 *   The suggestions parts.
 */
function _registryonsteroids_alter_extend_theme_wrappers_property_with_suggestions(array &$element, array $suggestions_parts) {
  if (!isset($element['#theme_wrappers']) || !is_array($element['#theme_wrappers'])) {
    return;
  }

  foreach ($element['#theme_wrappers'] as &$hook_or_hooks) {
    _registryonsteroids_alter_extend_value_with_suggestions(
      $hook_or_hooks,
      $suggestions_parts);
  }
}

/**
 * Extend the #theme property of an element.
 *
 * @param array $element
 *   The element.
 * @param string[] $suggestions_parts
 *   The suggestions parts.
 */
function _registryonsteroids_alter_extend_theme_property_with_suggestions(array &$element, array $suggestions_parts) {
  if (!isset($element['#theme'])) {
    return;
  }

  _registryonsteroids_alter_extend_value_with_suggestions(
    $element['#theme'],
    $suggestions_parts);
}

/**
 * Extends strings with an array of suggestions.
 *
 * @param string|string[] $hook_or_hooks
 *   The hook or hooks to extend.
 * @param string[] $suggestion_parts
 *   The suggestions to extend the base string with.
 *
 * @example
 *   $base = 'block';
 *   $suggestions_parts = ['system', 'main-menu'];
 *
 *   The result will be: 'block__system__main_menu'
 *
 *   Quick note about the drupal_strtolower() call:
 *   We must ensure lowercase values because get_defined_functions()
 *   in drupal_group_functions_by_prefix() returns functions in lower case.
 *
 * @see http://php.net/manual/en/function.get-defined-functions.php
 */
function _registryonsteroids_alter_extend_value_with_suggestions(&$hook_or_hooks, array $suggestion_parts) {
  $suggestion_parts = array_filter($suggestion_parts);

  if ([] === $suggestion_parts) {
    // Nothing to append.
    return;
  }

  // @todo Suffix calculation could be faster, if we would change the behavior a bit.
  $suffix = '';
  foreach ($suggestion_parts as $suggestion_part) {
    $suggestion_part = preg_replace('@\W@', '_', $suggestion_part);
    $suggestion_part = trim($suggestion_part, '_');
    if ('' === $suggestion_part) {
      continue;
    }
    $suffix .= '__' . $suggestion_part;
  }

  if ('' === $suffix) {
    return;
  }

  if (is_string($hook_or_hooks)) {
    $hook_or_hooks .= $suffix;
  }
  elseif (is_array($hook_or_hooks)) {
    // @todo Do we really want to append the suffix to every hook in the array?
    foreach ($hook_or_hooks as &$hook) {
      $hook .= $suffix;
    }
  }
}

/**
 * Custom helper function.
 *
 * This helper updates the render array of a block so
 * block preprocess callbacks are correctly processed.
 *
 * @param array $block_array
 *   The original block render array.
 * @param string|null $region
 *   The region where the block is placed.
 */
function _registryonsteroids_alter_update_block_render_array(array &$block_array, $region = NULL) {
  $suggestions_parts = [
    $region,
    $block_array['#block']->module,
    $block_array['#block']->delta,
  ];

  if (isset($block_array['#theme'])) {
    $suggestions_parts = array_merge($suggestions_parts, (array) $block_array['#theme']);
  }

  _registryonsteroids_alter_extend_theme_wrappers_property_with_suggestions(
    $block_array,
    $suggestions_parts);
}

/**
 * Custom utility callback that apply to each array element, a callback.
 *
 * Unlike array_walk_recursive, this function send the key name if a value is
 * an array.
 * As the value is passed by reference, you are able to modify its value within
 * the callback.
 *
 * @param array $variables
 *   The array.
 * @param callable $callback
 *   The callback function.
 */
function registryonsteroids_recursive_foreach(array &$variables, callable $callback) {
  $variables['#ros_processed'] = TRUE;

  foreach ($variables as $key => &$value) {
    if (is_array($value) && !isset($value['#ros_processed'])) {
      registryonsteroids_recursive_foreach($value, $callback);
    }

    call_user_func_array($callback, [$key, &$value, &$variables]);
  }

  unset($variables['#ros_processed']);
}

/**
 * Try to find information about the current page being viewed.
 *
 * This function will detect if an entity is displayed.
 * If an entity is displayed it will return an array containing:
 *  - The entity type
 *  - The entity info array
 *  - The entity itself.
 *
 * @return array|false
 *   The array if an entity is found, FALSE otherwise.
 */
function registryonsteroids_alter_menu_get_any_object() {
  $return = FALSE;
  $item = menu_get_item();
  $item['load_functions'] = (array) $item['load_functions'];
  $load_functions = array_values($item['load_functions']);

  if (NULL === $load_functions) {
    return $return;
  }

  $arg_position = array_search(
    $load_functions[0],
    $item['load_functions'],
    TRUE
  );

  foreach (entity_get_info() as $entity_name => $entity_info) {
    if ($entity_info['load hook'] !== $load_functions[0]) {
      continue;
    }

    if (($obj = menu_get_object($entity_name, $arg_position)) && is_object($obj)) {
      return [
        'entity' => $obj,
        'entity type' => $entity_name,
        'entity info' => $entity_info,
      ];
    }
  }

  return $return;
}

<?php

/**
 * @file
 * registryonsteroids_alter.alter.inc
 */

/**
 * Implements hook_entity_view_alter().
 */
function registryonsteroids_alter_entity_view_alter(array &$build, $type) {
  $suggestions_parts = [
    isset($build['#bundle']) ? $build['#bundle'] : NULL,
    isset($build['#view_mode']) ? $build['#view_mode'] : NULL,
  ];

  _registryonsteroids_alter_extend_theme_property_with_suggestions($build, $suggestions_parts);
  _registryonsteroids_alter_extend_theme_wrappers_property_with_suggestions($build, $suggestions_parts);
}

/**
 * Implements hook_form_alter().
 */
function registryonsteroids_alter_form_alter(array &$form, array &$form_state, $form_id) {
  $form['#pre_render'][] = '_registryonsteroids_alter_form_alter_pre_render';
}

/**
 * Custom pre render callback for hook_form_alter().
 *
 * @param array $form
 *   The form.
 *
 * @return array
 *   The form.
 */
function _registryonsteroids_alter_form_alter_pre_render(array $form) {
  $context = [
    'form' => $form,
    'suggestions parts' => [
      'form',
      $form['#id'],
    ],
  ];

  registryonsteroids_alter_recursive_element_children(
    $form,
    function (&$element, $element_key, &$context) {
      $suggestions = $context['suggestions parts'];
      if (isset($element['#type'])) {
        array_unshift($suggestions, $element['#type']);
      }

      _registryonsteroids_alter_extend_theme_property_with_suggestions(
        $element,
        array_merge($suggestions, $context['parents'])
      );
      _registryonsteroids_alter_extend_theme_wrappers_property_with_suggestions(
        $element,
        array_merge($suggestions, $context['parents'])
      );
    },
    NULL,
    $context
  );

  return $form;
}

/**
 * Implements hook_page_alter().
 *
 * @param array $page
 */
function registryonsteroids_alter_page_alter(array &$page) {
  $suggestions_parts = [];

  // Add a suggestion if the current page is the front page.
  if (drupal_is_front_page()) {
    $suggestions_parts[] = 'front';
  }

  // Add suggestions if an entity is found on the current page.
  if ($entity = registryonsteroids_alter_menu_get_any_object()) {
    $suggestions_parts[] = $entity['entity type'];

    if (isset($entity['entity']->{$entity['entity info']['entity keys']['bundle']})) {
      $suggestions_parts[] = $entity['entity']->{$entity['entity info']['entity keys']['bundle']};
    }
  }

  // Extend the #theme property of the page element.
  _registryonsteroids_alter_extend_theme_property_with_suggestions($page, $suggestions_parts);
  // Extend the #theme_wrappers property of the page element.
  _registryonsteroids_alter_extend_theme_wrappers_property_with_suggestions($page, $suggestions_parts);

  foreach (element_children($page) as $region_name) {
    // Extend the #theme_wrappers property of each page region.
    _registryonsteroids_alter_extend_theme_wrappers_property_with_suggestions(
      $page[$region_name],
      array_merge(
        (array) $region_name,
        $suggestions_parts));

    // Extend the blocks that are inside each region.
    registryonsteroids_recursive_foreach(
      $page[$region_name],
      function ($key, &$value, &$parent) use ($suggestions_parts) {
        if ('#block' !== $key) {
          return;
        }

        $block_suggestion_parts = [
          $value->region,
          $value->module,
          $value->delta,
        ];

        _registryonsteroids_alter_extend_theme_property_with_suggestions(
          $parent,
          array_merge(
            $block_suggestion_parts,
            $suggestions_parts));

        if (isset($parent['#theme'])) {
          $block_suggestion_parts = array_merge(
            (array) $parent['#theme'],
            $block_suggestion_parts);
        }

        _registryonsteroids_alter_extend_theme_wrappers_property_with_suggestions(
          $parent,
          array_merge(
            $block_suggestion_parts,
            $suggestions_parts));
      }
    );
  }
}

/**
 * Implements hook_field_attach_view_alter().
 *
 * @param array $output
 * @param array $context
 */
function registryonsteroids_field_attach_view_alter(array &$output, array $context = []) {
  // Array properties of the render array field to use to extend the field
  // #theme property.
  $suggestions_parts_names = [
    'field_type',
    'formatter',
    'field_name',
    'entity_type',
    'bundle',
    'view_mode',
  ];

  foreach (element_children($output) as $key) {
    $suggestions_parts_values = array_map(
      function ($suggestion) use ($output, $key) {
        return isset($output[$key]['#' . $suggestion]) ? $output[$key]['#' . $suggestion] : NULL;
      },
      $suggestions_parts_names);

    _registryonsteroids_alter_extend_theme_property_with_suggestions(
      $output[$key],
      $suggestions_parts_values
    );
  }
}

/**
 * Implements hook_contextual_links_view_alter().
 *
 * @param array $element
 * @param array $items
 */
function registryonsteroids_alter_contextual_links_view_alter(array &$element, array $items) {
  $suggestions_parts = [];

  if (isset($element['#element']['#theme'])) {
    $suggestions_parts = (array) $element['#element']['#theme'];
  }
  if (isset($element['#element']['#theme_wrappers'])) {
    $suggestions_parts = array_merge($suggestions_parts, $element['#element']['#theme_wrappers']);
  }

  _registryonsteroids_alter_extend_theme_property_with_suggestions($element, $suggestions_parts);
}

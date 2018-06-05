<?php

/**
 * @file
 * registryonsteroids_alter.menu.inc
 */

/**
 * Copy of the function menu_tree().
 *
 * This allows to pass a context variables to sub-functions calls.
 *
 * @param string $menu_name
 *   The menu name.
 * @param array $context
 *   The context.
 *
 * @see https://api.drupal.org/api/drupal/includes%21menu.inc/function/menu_tree/7.x
 *
 * @return mixed
 */
function registryonsteroids_menu_tree($menu_name, array $context = array()) {
  $menu_output = &drupal_static(__FUNCTION__, array());

  if (!isset($menu_output[$menu_name])) {
    $menu_output[$menu_name] = registryonsteroids_menu_tree_output(
      menu_tree_page_data($menu_name),
      $context
    );
  }

  return $menu_output[$menu_name];
}

/**
 * Copy of the function menu_tree_output().
 *
 * This allows to pass a context variables to sub-functions calls.
 *
 * @param array $tree
 *   The menu tree.
 * @param array $context
 *   The context.
 *
 * @see https://api.drupal.org/api/drupal/includes%21menu.inc/function/menu_tree_output/7.x
 *
 * @return array
 */
function registryonsteroids_menu_tree_output(array $tree, array $context = array()) {
  // Call to the original function.
  $build = menu_tree_output($tree);

  // Alteration of the menu build array.
  registryonsteroids_alter_recursive_menu_tree($build, function (&$element, $key, $recursive_context) use ($context) {
    $context += array(
      'suggestions' => array(),
      'level' => 0,
    );
    $context['suggestions'][] = 'level' . $recursive_context['level'];

    _registryonsteroids_alter_extend_theme_property_with_suggestions(
      $element,
      $context['suggestions']
    );
    _registryonsteroids_alter_extend_theme_wrappers_property_with_suggestions(
      $element,
      $context['suggestions']
    );
  });

  return $build;
}

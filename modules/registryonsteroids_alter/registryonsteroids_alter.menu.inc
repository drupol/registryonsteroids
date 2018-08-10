<?php

/**
 * @file
 * registryonsteroids_alter.menu.inc
 */

/**
 * Copy of the function menu_tree().
 *
 * It allows to pass a context variables.
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
function registryonsteroids_menu_tree($menu_name, array $context = []) {
  // Get the original menu tree static to avoid rebuilding it.
  $original_menu_tree = drupal_static('menu_tree', []);

  // Statically caching this function.
  $ros_menu_tree = &drupal_static(__FUNCTION__, []);

  if (!isset($ros_menu_tree[$menu_name])) {
    $context['menu_name'] = $menu_name;
    $context += ['suggestions parts' => []];

    // Get the original menu build array.
    $build = $original_menu_tree[$menu_name];

    // Alteration of the original menu build array.
    registryonsteroids_alter_recursive_menu_tree(
      $build,
      function (&$element, $key, $recursive_context) {
        _registryonsteroids_alter_add_default_pre_render_callback($element);
        _registryonsteroids_alter_add_default_pre_render_variables($element);

        $element['#registryonsteroids']['suggestions']['theme_wrappers'] = array_merge(
          $element['#registryonsteroids']['suggestions']['theme'],
          $element['#registryonsteroids']['suggestions']['theme_wrappers'],
          [
            $recursive_context['block']->module,
            $recursive_context['block']->delta,
            $recursive_context['block']->region,
            'level' . $recursive_context['level'],
          ],
          $recursive_context['suggestions parts']
        );

        $element['#registryonsteroids']['suggestions']['theme'] = array_merge(
          $element['#registryonsteroids']['suggestions']['theme'],
          [
            'level' . $recursive_context['level'],
          ],
          $recursive_context['suggestions parts']
        );
      },
      NULL,
      $context
    );

    $ros_menu_tree[$menu_name] = $build;
  }

  return $ros_menu_tree[$menu_name];
}

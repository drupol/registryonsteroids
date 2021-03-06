<?php

/**
 * @file
 * registryonsteroids_alter.alter.inc
 */

/**
 * Implements hook_field_attach_view_alter().
 */
function registryonsteroids_alter_field_attach_view_alter(array &$output, array $output_context = array()) {
  // Array properties of the render array field to use to extend the field
  // #theme property.
  $context = array(
    'suggestions parts names' => array(
      '#field_type',
      '#image_style',
      '#formatter',
      '#field_name',
      '#entity_type',
      '#bundle',
      '#view_mode',
    ),
  );

  registryonsteroids_alter_recursive_element_children(
    $output,
    function (&$element, $element_key, &$context) {
      $suggestions_parts_values = array_map(
        function ($suggestion) use ($element) {
          return isset($element[$suggestion]) ? $element[$suggestion] : NULL;
        },
        $context['suggestions parts names']
      );

      _registryonsteroids_alter_add_default_pre_render_variables($element);

      $element['#registryonsteroids']['suggestions']['theme'] = array_merge(
        $suggestions_parts_values,
        $element['#registryonsteroids']['suggestions']['theme']
      );
    },
    NULL,
    $context
  );
}

/**
 * Implements hook_entity_view_alter().
 */
function registryonsteroids_alter_entity_view_alter(array &$build, $type) {
  _registryonsteroids_alter_add_default_pre_render_callback($build);
  _registryonsteroids_alter_add_default_pre_render_variables($build);

  $suggestions_parts = array(
    isset($build['#bundle']) ? $build['#bundle'] : NULL,
    isset($build['#view_mode']) ? $build['#view_mode'] : NULL,
  );

  $build['#registryonsteroids']['suggestions']['theme'] = array_merge(
    $suggestions_parts,
    explode('/', $_GET['q'])
  );
}

/**
 * Implements hook_element_info_alter().
 */
function registryonsteroids_alter_element_info_alter(&$elements) {
  // Add a pre-render callback to every element.
  $elements = array_map(
    function ($element) {
      _registryonsteroids_alter_add_default_pre_render_callback($element);
      _registryonsteroids_alter_add_default_pre_render_variables($element);

      return $element;
    },
    $elements);
}

/**
 * Implements hook_form_alter().
 */
function registryonsteroids_alter_form_alter(array &$form, array &$form_state, $form_id) {
  $form['#pre_render'] = array_merge(
    array(
      '_registryonsteroids_alter_form_alter_pre_render' => '_registryonsteroids_alter_form_alter_pre_render',
    ),
    $form['#pre_render']
  );
}

/**
 * Implements hook_block_view_alter().
 */
function registryonsteroids_alter_block_view_alter(&$data, $block) {
  if ($data === NULL || array() === $data) {
    return;
  }

  $data += array(
    'content' => '',
  );

  if (empty($data['content'])) {
    return;
  }

  $block_theme_suggestions = array();
  _registryonsteroids_alter_add_default_pre_render_variables($data);

  // Specific handling for menus blocks.
  if ($block->module === 'system' || $block->module === 'menu') {
    switch ($block->module) {
      case 'system':
        $menus = menu_list_system_menus();

        break;

      case 'menu':
        $menus = menu_get_menus(FALSE);

        break;

      default:
        $menus = array();
    }

    // Update the block content.
    if (isset($menus[$block->delta])) {
      $context = array(
        'block' => $block,
        'suggestions parts' => array(),
      );

      // Alteration of the original menu build array.
      registryonsteroids_alter_recursive_menu_tree(
        $data['content'],
        function (&$element, $key, &$recursive_context) {
          _registryonsteroids_alter_add_default_pre_render_callback($element);
          _registryonsteroids_alter_add_default_pre_render_variables($element);

          $suggestions = array(
            $recursive_context['block']->module,
            $recursive_context['block']->delta,
            $recursive_context['block']->region,
            'level' . $recursive_context['level'],
          );

          $element['#registryonsteroids']['suggestions']['theme_wrappers'] = array_merge(
            $element['#registryonsteroids']['suggestions']['theme'],
            $element['#registryonsteroids']['suggestions']['theme_wrappers'],
            $suggestions,
            $recursive_context['suggestions parts']
          );

          $element['#registryonsteroids']['suggestions']['theme'] = array_merge(
            $element['#registryonsteroids']['suggestions']['theme'],
            $recursive_context['suggestions parts']
          );
        },
        NULL,
        $context
      );

      $content = $data['content'];

      // Add the contextual links if any.
      if (isset($data['content']['#contextual_links'])) {
        $content['#contextual_links'] = $data['content']['#contextual_links'];
      }

      $data['content'] = $content;

      // Delete this to avoid duplicates later.
      $block_theme_suggestions = (array) $data['content']['#registryonsteroids']['suggestions']['theme'];
    }
  }

  if (!is_array($data['content'])) {
    $data['content'] = array(
      '#markup' => $data['content'],
    );
  }

  _registryonsteroids_alter_add_default_pre_render_variables($data['content']);

  // We use array_merge() to not override existing data - ex: the user will be
  // able to add custom suggestions.
  $data['content']['#registryonsteroids']['suggestions']['theme'] = array_merge(
    array(
      $block->module,
      $block->delta,
      $block->region,
    ),
    $block_theme_suggestions
  );
  $data['content']['#registryonsteroids']['suggestions']['theme_wrappers'] = array_merge(
    array(
      $block->module,
      $block->delta,
      $block->region,
    ),
    $block_theme_suggestions
  );
}

/**
 * Implements hook_page_alter().
 */
function registryonsteroids_alter_page_alter(array &$page) {
  _registryonsteroids_alter_add_default_pre_render_variables($page);

  registryonsteroids_alter_recursive_element_children(
    $page,
    function (&$element, $element_key, &$recursive_context) {
      // Detect the region the element is in.
      $region = '';
      foreach ($recursive_context['parents'] as $parent) {
        if (isset($parent['#region'])) {
          $region = $parent['#region'];

          break;
        }
      }

      _registryonsteroids_alter_add_default_pre_render_variables($element);

      if (!isset($element['#markup'])) {
        _registryonsteroids_alter_add_default_pre_render_callback($element);
      }

      // Do not add twice the region to the blocks.
      if (!isset($element['#block'])) {
        $element['#registryonsteroids']['suggestions']['theme_wrappers'] = array_merge(
          array($region),
          $element['#registryonsteroids']['suggestions']['theme_wrappers']
        );
      }

      $element['#registryonsteroids']['suggestions']['theme'] = array_merge(
        array($region),
        $element['#registryonsteroids']['suggestions']['theme']
      );
    }
  );
}

/**
 * Implements hook_contextual_links_view_alter().
 */
function registryonsteroids_alter_contextual_links_view_alter(array &$element, array $items) {
  $suggestions_parts = array();

  if (isset($element['#element']['#theme'])) {
    $suggestions_parts = array_merge(
      $suggestions_parts,
      (array) $element['#element']['#theme']
    );
  }

  if (isset($element['#element']['#block'])) {
    $suggestions_parts = array_merge(
      $suggestions_parts,
      $element['#element']['#block']->{'#registryonsteroids'}['suggestions']['theme']
    );
  }

  registryonsteroids_alter_recursive_element_children(
    $element,
    function (&$element) use ($suggestions_parts) {
      $element['#registryonsteroids']['suggestions']['theme'] = array_merge(
        $element['#registryonsteroids']['suggestions']['theme'],
        $suggestions_parts);
    }
  );
}

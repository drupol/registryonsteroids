<?php

/**
 * @file
 * registryonsteroids_alter.alter.inc
 */

/**
 * Implements hook_element_info_alter().
 */
function registryonsteroids_alter_element_info_alter(&$types) {
  foreach ($types as &$type) {
    if (isset($type['#theme_wrappers'])) {
      foreach ((array) $type['#theme_wrappers'] as $key => $theme_wrapper) {
        if ($type['#type'] !== $theme_wrapper) {
          // Using current to make sure to have the first item of the array
          // Instead of the complete array.
          $type['#theme_wrappers'][$key] = current(
            _registryonsteroids_alter_extend_with_suggestions(
              array($theme_wrapper),
              array($type['#type'])
            )
          );
        }
      }
    }
  }
}

/**
 * Implements hook_entity_view_alter().
 */
function registryonsteroids_alter_entity_view_alter(&$build, $type) {
  $build['#theme'] = _registryonsteroids_alter_extend_theme_hook($build['#theme'], array($build['#bundle'], $build['#view_mode']));
}

/**
 * Implements hook_form_alter().
 */
function registryonsteroids_alter_form_alter(&$form, &$form_state, $form_id) {
  // Custom recursive function to alter all the #theme keys of form elements.
  _registryonsteroids_alter_form_element_theme_alter($form, $form_state, $form_id);

  /*
   * Prevent user-facing field styling from screwing up node edit forms by
   * renaming the classes on the node edit form's field wrappers.
   *
   * Remove if #1245218 is backported to D7 core.
   */
  foreach (array_keys($form) as $item) {
    if (strpos($item, 'field_') === 0) {
      if (!empty($form[$item]['#attributes']['class'])) {
        foreach ($form[$item]['#attributes']['class'] as &$class) {
          // Core bug: the field-type-text-with-summary class
          // is used as a JS hook.
          if (($class !== 'field-type-text-with-summary') && (0 === strpos($class, 'field-type-')|| strpos($class, 'field-name-') === 0)) {
            // Make the class different from that used in theme_field().
            $class = 'form-' . $class;
          }
        }
      }
    }
  }
}

/**
 * Custom callback.
 *
 * This function will alter all the element in a form and update their
 * #theme key to append the $form_id at the end so we can preprocess them.
 *
 * @see form_builder()
 */
function _registryonsteroids_alter_form_element_theme_alter(&$form, &$form_state, $form_id) {
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['#type']) && empty($form[$key]['#defaults_loaded']) && ($info = element_info($form[$key]['#type']))) {
      if (isset($info['#theme'])) {
        $form[$key]['#theme'] = _registryonsteroids_alter_extend_with_suggestions(
          (array) $info['#theme'],
          array('form', $form_id, $key)
        );
      }

      if (isset($info['#theme_wrappers'])) {
        $form[$key] += array('#theme_wrappers' => array());
        $form[$key]['#theme_wrappers'] = _registryonsteroids_alter_extend_with_suggestions(
          $info['#theme_wrappers'],
          array('form', $form_id, $key)
        );
      }
    }

    _registryonsteroids_alter_form_element_theme_alter($form[$key], $form_state, $form_id);
  }
}

/**
 * Implements hook_page_alter().
 */
function registryonsteroids_alter_page_alter(&$page) {
  foreach (system_region_list($GLOBALS['theme'], REGIONS_VISIBLE) as $region => $name) {
    if (empty($page[$region])) {
      continue;
    }

    if (isset($page[$region]['#theme_wrappers'])) {
      $page[$region]['#theme_wrappers'] = _registryonsteroids_alter_extend_with_suggestions($page[$region]['#theme_wrappers'], array($region));
    }

    $block_index_in_region = 0;
    $blocks = element_children($page[$region]);
    $total_blocks_in_region = count($blocks);

    // Alter each blocks to provides relevant variables in the block preprocess.
    foreach ($blocks as $block) {
      _registryonsteroids_alter_update_block_render_array($page[$region][$block], $region, $block_index_in_region++, $total_blocks_in_region);
    }
  }
}

/**
 * Implements hook_field_attach_view_alter().
 */
function atomium_field_attach_view_alter(&$output, $context) {
  $suggestion_data = array(
    'field_type',
    'formatter',
    'field_name',
    'entity_type',
    'view_mode',
    'bundle',
  );

  foreach (element_children($output) as $key) {
    $suggestions = array_filter(array_map(function ($suggestion) use ($output, $key) {
      return isset($output[$key]['#' . $suggestion]) ? $output[$key]['#' . $suggestion] : NULL;
    }, $suggestion_data));

    if (isset($output[$key]['#theme']) && !empty($suggestions)) {
      $output[$key]['#theme'] = _registryonsteroids_alter_extend_with_suggestions(
        (array) $output[$key]['#theme'],
        $suggestions
      );
    }
  }
}

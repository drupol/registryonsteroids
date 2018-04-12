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
 */
function registryonsteroids_alter_page_alter(array &$page) {
  $suggestions_parts = [];

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

  $context = [
    'suggestions parts' => $suggestions_parts,
    'region' => '',
  ];

  registryonsteroids_alter_recursive_element_children(
    $page,
    function (&$element, $element_key, &$context) {
      $suggestions_parts = $context['suggestions parts'];

      if (isset($element['#region'])) {
        $region_suggestions_parts = $suggestions_parts;
        array_unshift($region_suggestions_parts, $element['#region']);

        _registryonsteroids_alter_extend_theme_wrappers_property_with_suggestions(
          $element,
          $region_suggestions_parts
        );

        $context['region'] = $element['#region'];
      }

      if (isset($element['#block'])) {
        $block_suggestion_parts = array_merge(
          [$element['#block']->module, $element['#block']->delta],
          [$context['region']],
          $suggestions_parts);

        _registryonsteroids_alter_extend_theme_property_with_suggestions(
          $element,
          $block_suggestion_parts
        );

        _registryonsteroids_alter_extend_theme_wrappers_property_with_suggestions(
          $element,
          $block_suggestion_parts
        );
      }
    },
    NULL,
    $context
  );
}

/**
 * Implements hook_field_attach_view_alter().
 */
function registryonsteroids_alter_field_attach_view_alter(array &$output, array $output_context = []) {
  // Array properties of the render array field to use to extend the field
  // #theme property.
  $context = [
    'suggestions parts names' => [
      '#field_type',
      '#formatter',
      '#field_name',
      '#entity_type',
      '#bundle',
      '#view_mode',
    ],
  ];

  registryonsteroids_alter_recursive_element_children(
    $output,
    function (&$element, $element_key, &$context) {
      $suggestions_parts_values = array_map(
        function ($suggestion) use ($element, $element_key) {
          return isset($element[$element_key][$suggestion]) ? $element[$element_key][$suggestion] : NULL;
        },
        $context['suggestions parts names']);

      _registryonsteroids_alter_extend_theme_property_with_suggestions(
        $element,
        $suggestions_parts_values
      );
    },
    NULL,
    $context
  );
}

/**
 * Implements hook_contextual_links_view_alter().
 */
function registryonsteroids_alter_contextual_links_view_alter(array &$element, array $items) {
  $suggestions_parts = [];

  if (isset($element['#element']['#theme'])) {
    $suggestions_parts = (array) $element['#element']['#theme'];
  }
  if (isset($element['#element']['#theme_wrappers'])) {
    $suggestions_parts = array_merge($suggestions_parts, $element['#element']['#theme_wrappers']);
  }

  $context = [
    'suggestions parts' => $suggestions_parts,
  ];

  registryonsteroids_alter_recursive_element_children($element, function (&$element, $element_key, &$context) {
    _registryonsteroids_alter_extend_theme_property_with_suggestions(
      $element,
      $context['suggestions parts']
    );
  }, NULL, $context);
}

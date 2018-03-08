<?php

/**
 * @file
 * registryonsteroids_alter.helpers.inc
 */

/**
 * Extends strings with an array of suggestions.
 *
 * @param array $items
 *   The strings to extend.
 * @param array $suggestions
 *   The suggestions to add.
 *
 * @example
 *   $items = array('container', 'block');
 *   $suggestions = array('foo', 'bar');
 *
 *   The result will be: array('container__foo__bar', 'block__foo__bar')
 *
 *   Quick note about the drupal_strtolower() call:
 *   We must ensure lowercase values because get_defined_functions()
 *   in drupal_group_functions_by_prefix() returns functions in lower case.
 *
 * @see http://php.net/manual/en/function.get-defined-functions.php
 *
 * @return array
 *   The extended strings.
 */
function _registryonsteroids_alter_extend_with_suggestions(array $items = array(''), array $suggestions = array()) {
  return array_map(function ($base_hook) use ($suggestions) {
    if ($hook = _registryonsteroids_alter_extend_theme_hook($base_hook, $suggestions)) {
      return $hook[0];
    }

    return NULL;
  }, $items);
}

/**
 * Extend a theme hook with suggestions.
 *
 * @param string|null $base
 *   The base hook.
 * @param string|array $suggestions
 *   The suggestions.
 *
 * @return array|false
 *   An array containing the theme hook extended,
 *   FALSE if the base hook is null.
 */
function _registryonsteroids_alter_extend_theme_hook($base = NULL, $suggestions = array()) {
  if (empty($base) || !is_string($base)) {
    return FALSE;
  }

  $suggestions = (array) $suggestions;
  array_unshift($suggestions, $base);

  return array(
    str_replace(
      '-',
      '_',
      drupal_strtolower(
        implode(
          '__',
          array_filter(
            $suggestions
          )
        )
      )
    ),
  );
}

/**
 * Custom helper function.
 *
 * This helper updates the render array of a block so block preprocess are
 * correctly processed.
 *
 * @param array $block_array
 *   The original block render array.
 * @param string|null $region
 *   The region where the block is placed.
 * @param int $block_index_in_region
 *   The position of the block in the region, if any.
 * @param int $total_blocks_in_region
 *   The total number of blocks in the region.
 */
function _registryonsteroids_alter_update_block_render_array(array &$block_array, $region = NULL, $block_index_in_region = 0, $total_blocks_in_region = 0) {
  $block_array['#block']->atomium['block_index_in_region'] = $block_index_in_region;
  $block_array['#block']->atomium['total_blocks_in_regions'] = $total_blocks_in_region;

  if (isset($block_array['#theme_wrappers'])) {
    $block_array['#theme_wrappers'] = _registryonsteroids_alter_extend_with_suggestions(
      $block_array['#theme_wrappers'],
      array(
        is_string($region) ? $region : NULL,
        $block_array['#block']->module,
        $block_array['#block']->delta,
      )
    );
  }
}

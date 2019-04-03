<?php

/**
 * @file
 * Theme template file.
 */

/**
 * Implements hook_preprocess_hook().
 */
function ros_theme_preprocess_ros2(&$variables, $hook) {
  $variables['callbacks'][] = __FUNCTION__;
}

/**
 * Implements hook_preprocess_hook().
 */
function ros_theme_preprocess_ros2__variant1(&$variables, $hook) {
  $variables['callbacks'][] = __FUNCTION__;
}

/**
 * Implements hook_process_hook().
 */
function ros_theme_process_ros2__variant2__foo(&$variables, $hook) {
  $variables['callbacks'][] = __FUNCTION__;
}

/**
 * Implements hook_preprocess_hook().
 */
function ros_theme_preprocess_ros4(&$variables, $hook) {
  $variables['callbacks'][] = __FUNCTION__;
}

/**
 * Implements hook_preprocess_hook().
 */
function ros_theme_preprocess_ros4__foo(&$variables, $hook) {
  $variables['callbacks'][] = __FUNCTION__;
}

/**
 * Implements hook_theme().
 */
function ros_theme_theme() {
  // The purpose of this hook is to test the registry definition when a hook
  // is overriden and updated.
  // In this case, we override the item_list hook and add more variables to it.
  // When the registry is built, all the variants of this hook should have the
  // same variables.
  return array(
    'item_list' => array(
      'variables' => array(
        'items' => array(),
        'title' => NULL,
        'type' => 'ul',
        'attributes' => array(),
        // These two following variables were manually added.
        'foo' => 'bar',
        'bar' => 'foo',
      ),
    ),
  );
}

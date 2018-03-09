<?php

/**
 * @file
 * Theme template file.
 */

/**
 * Implements hook_preprocess_hook().
 */
function ros_theme_preprocess_ros2(&$variables, $hook) {
  $variables['text'] .= __FUNCTION__ . ',';
}

/**
 * Implements hook_preprocess_hook().
 */
function ros_theme_preprocess_ros2__variant1(&$variables, $hook) {
  $variables['text'] .= __FUNCTION__ . ',';
}

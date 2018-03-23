<?php

namespace Drupal\registryonsteroids;

/**
 * Class ThemeRegistryAltererFirst.
 */
final class ThemeRegistryAltererFirst implements ThemeRegistryAltererInterface {

  /**
   * The Drupal's module list.
   *
   * @var array
   */
  public $moduleList;

  /**
   * The base themes.
   *
   * @var object[]
   */
  public $baseThemes;

  /**
   * The current theme.
   *
   * @var object
   */
  public $theme;

  /**
   * Constructor.
   *
   * @param array $module_list
   *   The Drupal's module list.
   * @param object[] $base_themes
   *   The base themes.
   * @param object $theme
   *   The current theme.
   */
  public function __construct(array $module_list = array(), array $base_themes = array(), $theme = NULL) {
    $this->moduleList = $module_list;
    $this->baseThemes = $base_themes;
    $this->theme = $theme;
  }

  /**
   * {@inheritdoc}
   */
  public function alter(array &$registry) {
    ksort($registry);

    $stubs = $this->buildStubs($registry);

    ksort($stubs);

    $registry = array();
    foreach ($stubs as $hook => $stub) {
      $registry[$hook] = $stub->getRegistryEntry();
    }
  }

  /**
   * Build the theme registry stubs.
   *
   * @param array $registry
   *   The Drupal's registry.
   *
   * @return \Drupal\registryonsteroids\ThemeHookStub[]
   */
  private function buildStubs(array $registry) {
    $functions_by_hook_and_phasekey_and_weight = FunctionGroupUtil::groupFunctionsByHookAndPhasekeyAndWeight(
      get_defined_functions()['user'],
      array_merge(...array_values($this->makePrefixes($this->moduleList, $this->baseThemes, $this->theme)))
    );
    $functions_by_hook_and_phasekey_and_weight += array('*' => array());

    $declared_hooks = array_keys($registry);
    $declared_hooks = array_combine($declared_hooks, $declared_hooks);

    $declared_base_hooks = array();
    $declared_root_hooks = array();
    foreach ($registry as $hook => $info) {
      if (isset($info['base hook']) && $hook !== $info['base hook']) {
        $declared_base_hooks[$hook] = $info['base hook'];
      }
      else {
        $declared_root_hooks[$hook] = $hook;
      }
    }

    $discovered_hooks = array_keys($functions_by_hook_and_phasekey_and_weight);
    $discovered_hooks = array_combine($discovered_hooks, $discovered_hooks);

    $factory = new ThemeHookStubFactory(
      $registry,
      $functions_by_hook_and_phasekey_and_weight);

    return $this->doBuildStubs(
      $factory,
      $declared_hooks + $discovered_hooks,
      $declared_base_hooks,
      $declared_root_hooks);
  }

  /**
   * Builds theme registry stubs from prepared parameters.
   *
   * This was split out of the parent function to reduce function sizes, and
   * "because we can".
   *
   * @param \Drupal\registryonsteroids\ThemeHookStubFactory $factory
   * @param string[] $hooks
   *   Format: $[$hook] = $hook
   * @param string[] $base_hooks
   *   Format: $[hook] = $base_hook
   * @param string[] $declared_root_hooks
   *   Format: $[$hook] = $hook
   *
   * @return \Drupal\registryonsteroids\ThemeHookStub[]
   */
  private function doBuildStubs(ThemeHookStubFactory $factory, array $hooks, array $base_hooks, array $declared_root_hooks) {
    $sortme = [];
    foreach ($hooks as $hook) {
      $sortme[$hook . '__'] = $hook;
      $sortme[$hook . '__|'] = FALSE;
    }

    ksort($sortme);

    /** @var \Drupal\registryonsteroids\ThemeHookStub[] $trail */
    $trail = array();

    /** @var \Drupal\registryonsteroids\ThemeHookStub[] $stubs */
    $stubs = array();
    /** @var \Drupal\registryonsteroids\ThemeHookStub|null $stub */
    $stub = NULL;

    $base_hook = '*';
    foreach ($sortme as $hook_or_false) {
      if (FALSE === $hook_or_false) {
        $stub = array_pop($trail);
        continue;
      }

      $trail[] = $stub;
      $hook = $hook_or_false;

      if (isset($stubs[$hook])) {
        $stub = $stubs[$hook];
        continue;
      }

      if (isset($declared_root_hooks[$hook])) {
        // $hook has no parents.
        $stub = $factory->createStub($hook);
      }
      elseif (!isset($base_hooks[$hook]) || $base_hook === $base_hooks[$hook]) {
        // $hook has the existing $stub as a parent, if that exists.
        if (NULL === $stub) {
          // This is a discovered hook that is not an ancestor
          // of a declared hook.
          continue;
        }
        $stub = $factory->createStub($hook, $stub);
      }
      else {
        // $hook has one base hook but no other parents.
        $base_hook = $base_hooks[$hook];
        if (isset($stubs[$base_hook])) {
          // Base hook was already calculated.
          $stub = $factory->createStub($hook, $stubs[$base_hook]);
        }
        elseif (isset($declared_root_hooks[$base_hook])) {
          // Base hook is a known root hook, we just haven't visited it yet.
          $base_stub = $factory->createStub($base_hook);
          $stub = $factory->createStub($hook, $base_stub);
        }
        else {
          // This is an invalid entry in theme registry.
          // Treat this as if it didn't have a base hook.
          $stub = $factory->createStub($hook);
        }
      }

      if (NULL !== $stub) {
        $stubs[$hook] = $stub;
      }
    }

    return $stubs;
  }

  /**
   * Compile a list of prefixes.
   *
   * The order of this is very important.
   *
   * @param array $module_list
   *   The module list.
   * @param object[] $base_themes
   *   The array of base themes.
   * @param object $theme
   *   The current theme.
   *
   * @see https://api.drupal.org/api/drupal/includes!theme.inc/function/theme/7.x
   *
   * @return string[][]
   *   The prefixes.
   *   Format: $[$prefix_type][$prefix] = $prefix
   */
  private function makePrefixes(array $module_list, array $base_themes, $theme) {
    /** @var string[] $base_theme_names */
    $base_theme_names = array_map(
      function ($theme) {
        return $theme->name;
      },
      $base_themes);

    return array(
      'template' => array(
        'template' => 'template',
      ),
      'module' => $module_list,
      'theme_engine' => array(
        $theme->engine => $theme->engine,
      ),
      'base_theme' => array_combine(
        $base_theme_names,
        $base_theme_names
      ),
      'theme' => array(
        $theme->name => $theme->name,
      ),
    );
  }

}

<?php

namespace Drupal\Tests\registryonsteroids\Kernel;

use Symfony\Component\Yaml\Yaml;

/**
 * Class CoreRegistryDefinitionsTest.
 */
class CoreRegistryDefinitionsTest extends AbstractThemeTest {

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();
    module_disable(array('registryonsteroids'));
    drupal_flush_all_caches();
  }

  /**
   * Test the registry definition validity.
   *
   * @dataProvider registryProvider
   */
  public function testRegistryDefinitions($hook, $definition) {
    $registry = theme_get_registry(TRUE);

    $this->assertArrayHasKey($hook, $registry);
    $this->assertEquals($definition, $registry[$hook]);
  }

  /**
   * Test the registry definition against the missing definitions.
   *
   * @dataProvider registryMissingProvider
   */
  public function testMissingRegistryDefinitions($hook) {
    $registry = theme_get_registry(TRUE);
    $this->assertArrayNotHasKey($hook, $registry);
  }

  /**
   * Test the render of a theme hook.
   *
   * @dataProvider renderProvider
   */
  public function testPreprocessProcessCascade($hook, $render) {
    $this->assertEquals(
      "\n" . implode("\n", $render) . "\n",
      "\n" . str_replace(',', "\n", theme($hook)));
  }

  /**
   * Return registry fixtures.
   *
   * @return array
   *   List of registry fixtures.
   */
  public function registryProvider() {
    $argss_indexed = Yaml::parseFile(__DIR__ . '/../../fixtures/core/registry/registry.yml');
    $argss = array();
    foreach ($argss_indexed as $i => $args) {
      $argss[$i . ': ' . $args['hook']] = $args;
    }
    return $argss;
  }

  /**
   * Return registry fixtures that should exist in core but doesn't.
   *
   * @return array
   *   List of registry fixtures.
   */
  public function registryMissingProvider() {
    return Yaml::parseFile(__DIR__ . '/../../fixtures/core/registry/registry_missing.yml');
  }

  /**
   * Return render fixtures.
   *
   * @return array
   *   List of registry fixtures.
   */
  public function renderProvider() {
    $argss_indexed = Yaml::parseFile(__DIR__ . '/../../fixtures/core/render/render.yml');
    $argss = array();
    foreach ($argss_indexed as $i => $args) {
      $argss[$i . ': ' . $args['hook']] = $args;
    }
    return $argss;
  }

}

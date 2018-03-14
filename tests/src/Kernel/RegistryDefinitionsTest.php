<?php

namespace Drupal\Tests\registryonsteroids\Kernel;

use Symfony\Component\Yaml\Yaml;

/**
 * Class RegistryDefinitionsTest.
 */
class RegistryDefinitionsTest extends AbstractThemeTest {

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();
    module_enable(array('registryonsteroids'));
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
    $argss_indexed = Yaml::parseFile(__DIR__ . '/../../fixtures/ros/registry/registry.yml');
    $argss = array();
    foreach ($argss_indexed as $i => $args) {
      $argss[$i . ': theme(' . var_export($args['hook'], TRUE) . ')'] = $args;
    }
    return $argss;
  }

  /**
   * Return render fixtures.
   *
   * @return array
   *   List of registry fixtures.
   */
  public function renderProvider() {
    $argss_indexed = Yaml::parseFile(__DIR__ . '/../../fixtures/ros/render/render.yml');
    $argss = array();
    foreach ($argss_indexed as $i => $args) {
      $argss[$i . ': ' . $args['hook']] = $args;
    }
    return $argss;
  }

}

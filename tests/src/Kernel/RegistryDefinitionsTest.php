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
      "\n" . str_replace(',', "\n", $render),
      "\n" . str_replace(',', "\n", theme($hook)));
  }

  /**
   * Return registry fixtures.
   *
   * @return array
   *   List of registry fixtures.
   */
  public function registryProvider() {
    return Yaml::parseFile(__DIR__ . '/../../fixtures/ros/registry/registry.yml');
  }

  /**
   * Return render fixtures.
   *
   * @return array
   *   List of registry fixtures.
   */
  public function renderProvider() {
    return Yaml::parseFile(__DIR__ . '/../../fixtures/ros/render/render.yml');
  }

}

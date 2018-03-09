<?php

namespace Drupal\Tests\registryonsteroids\Kernel;

use Symfony\Component\Yaml\Yaml;

/**
 * Class RegistryDefinitionsTest.
 */
class RegistryDefinitionsTest extends AbstractThemeTest {

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
    $this->assertEquals($render, theme($hook));
  }

  /**
   * Return registry fixtures.
   *
   * @return array
   *   List of registry fixtures.
   */
  public function registryProvider() {
    return Yaml::parseFile(__DIR__ . '/../../fixtures/registry/registry.yml');
  }

  /**
   * Return render fixtures.
   *
   * @return array
   *   List of registry fixtures.
   */
  public function renderProvider() {
    return Yaml::parseFile(__DIR__ . '/../../fixtures/render/render.yml');
  }

}

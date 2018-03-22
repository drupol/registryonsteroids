[![Build Status](https://travis-ci.org/drupol/registryonsteroids.svg?branch=7.x-1.x)](https://travis-ci.org/drupol/registryonsteroids)

# Registry On Steroïds

Registry On Steroïds(ROS) discovers and adds additional theme preprocess/process functions for theme hook variants, if `theme()` is called with a variant name. E.g. for `theme('node__article__teaser', ..)`, it will call functions like `MYTHEME_preprocess_node__article__teaser()` and `MYTHEME_preprocess_node__article()` in addition to `MYTHEME_preprocess_node()`.

Without this module, only the process/preprocess functions of the base hook will be called, when a theme hook variant is executed. E.g. for `theme('node__article__teaser')`, only the preprocess and process functions for `'node'` are called. See [#2563445](https://www.drupal.org/node/2563445) in the issue queue for Drupal 7.

The module only has en effect if the theme hook is called with a variant hook name. It does not work for theme hook suggestions added to the `$variables` array.

## Background: Theme hook variants

A "theme hook variant" is a specialized version of a base theme hook, with a suffix like "__$x", that is executed instead of the base hook in specific cases.

Based on such theme hook variants, it might execute a specific template `node--article--teaser.tpl.php`, if such a template exists in the active theme or in a module. Otherwise it will fall back to the parent or base template, e.g. `node--article.tpl.php` or `node.tpl.php`. Or, for theme functions, it might execute `THEMENAME_menu_tree__main_menu()` instead of the base function `THEMENAME_menu_tree()` or `theme_menu_tree()`.

Drupal 7 has 4 ways of invoking theme hook variants:
1. The `theme()` function can be called with an array of theme hooks, instead of a single theme hook, e.g. `theme(array('node__article__teaser', 'node__article', 'node'), ..)`.
2. The `theme()` function can be called with a hook name containing double underscores, e.g. `theme('node__article__teaser')`. Drupal will try different substrings of the specified hook, until it finds an existing registered theme hook variant.
3. The `theme()` function can be called with theme hook suggestions in the `$variables` array, e.g. `theme('node', $variables)` with `$variables['theme_hook_suggestions'] === array('node__article__teaser', 'node__article')`.
4. After the `theme()` function is called, possibly with just the base hook `'node'`, preprocses functions can register an array of theme hook names or theme hook variant names in `$variables['theme_hook_suggestions']`. Later, these suggestions will be used to determine which template should be rendered, or which theme function should be executed.

The last is currently the most common way to invoke theme hook variants in Drupal 7.

In the theme registry, a theme hook variant is an entry with an `$info['base hook']` setting, pointing to another theme hook. E.g. the entry for `'node__article__teaser'`, if it exists, would have `$registry['node__article__teaser']['base hook'] === 'node'`.

The base theme hooks themselves, e.g. `'node'`, do not have a `'base hook'` setting.


## Background: (Pre)process functions

Before calling the theme function or including the template, `theme()` will excute a series of preprocess and process functions registered for this theme hook. The functions are discovered based on their function name in a clearly defined order, but this array can be modified by modules with `hook_theme_registry_alter()`.

E.g. for `theme('node', ..)`, all existing functions like `template_preprocess_node()`, `MYMODULE_preprocess_node()` or `MYTHEME_preprocess_node()` will be executed.

Some modules use `hook_theme_registry_alter()` to register additional functions for specific theme hooks, e.g. [Display Suite](https://drupal.org/project/ds) registers a function `ds_entity_variables()` as a preprocessor for all entity theme hooks.

When calling a theme hook variant like `theme('node__article__teaser', ..)`, only the base hook preprocess functions like `MYTHEME_preprocess_node()` are executed. Variant preprocess functions like `MYTHEME_preprocess_node__article__teaser()` are not discovered and not called.

The variant preprocess functions used to work (more or less) in Drupal 6, so this could be seen as a regression in Drupal 7.

## This module

This module modifies the theme registry in the following way:
- Entries for base hooks are not changed.
- Existing entries for variants, e.g. if a template like `node--article--teaser.tpl.php` was discovered, are modified so that variant-specific preprocess and process functions are called in addition to those of the base hook. Functions that were added by contrib modules, like `ds_entity_variables` added by Display Suite, are preserved, as if the module had added the function on the variant itself.
- New variant entries are created for discovered preprocess/process functions where an entry does not exist yet. E.g. if a preprocess function `MYTHEME_preprocess_node__webform__full` is found, then a new entry for `$registry['node__webform__full']` will be created, even if no template like `node--webform--full.tpl.php` exists.


## Submodule "registryonsteroids_alter"

The submodule modifies render arrays, so that `$element['#theme']` refers to a hook variant instead of a base hook. Without this, the base module would have little effect.


# Details

When it comes to render a theme hook, through [a render array](https://www.drupal.org/docs/7/api/render-arrays/render-arrays-overview) or [the theme function](https://api.drupal.org/api/drupal/includes!theme.inc/function/theme/7.x), the Drupal's 7 default behavior is to run a set of callbacks for preprocess and a set of callback for process in [a particular order](https://api.drupal.org/api/drupal/includes!theme.inc/function/theme/7.x).

Example:

You're using the Bartik core theme and you want to render the theme hook `node` and add some variants like its bundle name and its view mode.

Instead of using `theme('node', [...]);`, you will use `theme('node__page__full', [...]);`.

Then, in your theme or module, you create a preprocess function: `[HOOK]_preprocess_node__page__full(&$variables, $hook);`.

When rendering your node, Drupal will run the preprocess callbacks in the following order:

* [template_preprocess()](https://api.drupal.org/api/drupal/includes%21theme.inc/function/template_preprocess/7.x)
* [template_preprocess_node()](https://api.drupal.org/api/drupal/modules%21node%21node.module/function/template_preprocess_node/7.x)
* [bartik_preprocess_node()](https://api.drupal.org/api/drupal/themes%21bartik%21template.php/function/bartik_preprocess_node/7.x)
* [HOOK]_preprocess_node__page__full()

Once those preprocess are executed, Drupal will try to render the theme hook.
The theme hook `node__page__full` doesn't exist per se, so Drupal will try to render its parent: `node__page`, but in our case, it doesn't exist either.
So Drupal will iterate until a valid theme hook is found, in this case: `node`.

So far so good.

But What happens if you want to apply the same preprocessing to all the node of type page regardless of the view mode ?

The first idea is to create a specific preprocess: `[HOOK]_preprocess_node__page(&$variables, $hook)`
Unfortunately, this preprocess will be completely ignored by Drupal.

This module fixes this behavior and let Drupal use intermediary or derivative preprocess/process callbacks.

An issue is open since on drupal.org to fix this behavior, see [#2563445](https://www.drupal.org/node/2563445).

This modules provides a configuration form where you can enable or disable the `theme_debug` option available [since Drupal 7.33](https://www.drupal.org/node/223440#theme-debug) and enable the rebuild of the registry at each page load.
             
# Submodules

**Registry On Steroïds Alter** updates(alter!) default Drupal's render arrays and extends their `#theme` and `#theme_wrappers` members.

Example:

When calling `theme('node', [...]);` to render a page node, ROS will alter the render array and in the end,
`theme('node__page__full', [...]);` will be used to render the page node.

This will allow themers and designers to use particular preprocess/process callbacks like the following in this order:

* `[HOOK]_preprocess_node(&$variables, $hook);`
* `[HOOK]_preprocess_node__page(&$variables, $hook);`
* `[HOOK]_preprocess_node__page__full(&$variables, $hook);`

# History

The code of this module comes from [Atomium](https://www.drupal.org/project/atomium), a Drupal 7 base theme that implements all of this in a theme.
The code of Atomium is inspired by the code of many themes, especially [Bootstrap](https://www.drupal.org/project/bootstrap).

The idea behind this module is to remove the code that alter the theme registry from the theme and make it available for anyone through a module so every theme can enjoy these enhancements.

# Issues to follow

This module is watching issues on drupal.org. Once these issues are fixed, we'll be able to update this module and hopefully deprecate it at a certain point.

* [#1119364](https://www.drupal.org/node/1119364)
* [#1545964](https://www.drupal.org/node/1545964)
* [#2563445](https://www.drupal.org/node/2563445)

Feel free to test patches from these issues and give your feedback so we can move them forward.

# Tests

To run the tests locally:

* `git clone https://github.com/drupol/registryonsteroids.git`
* `composer install`

Then if you want to modify the default settings of the Drupal installation, please copy the file `runner.yml.dist` into `runner.yml` and update that file according to your configuration.

* `./vendor/bin/run drupal:site-install`

At this point, the Drupal installation will be in the `build` directory.

To run the test properly, you must make sure that a web server is started on this directory.

Run this command in a new terminal: `cd build; ../vendor/bin/drush rs`

Then, you are able to run the tests:

* `./vendor/bin/grumphp run`

# Author

* [Pol Dellaiera](http://drupal.org/u/pol)

# Contributors

* [Mark Carver](https://www.drupal.org/u/markcarver)
* [Andreas Hennings](https://www.drupal.org/u/donquixote)

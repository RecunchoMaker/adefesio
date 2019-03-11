---
layout: post
title:  "Jekyll local"
---
Ee post no tiene nada que ver con el proyecto del robot en si, pero ya que estamos
documentando el proceso de creación, vamos a documentar también el propio proceso
de documentación.

# Instalación de jekyll

Estamos utilizando github pages para la creación de este blog, y a partir de un
esqueleto simple hemos empezado a poner posts en la carpeta correspondiente. Sin
embargo, hemos tenido errores que no detectamos hasta el momento en el que github
renderiza las páginas, provocando que tengamos que hacer push de corrección
que ensucian la historia de commits innecesariamente.

Así que vamos a instalar jekyll de forma local para poder visualizar inmediatamente
el resultado de los posts y de paso poder introducir facilmente alguna mejora de maquetación.

En la propia página de jekyll se especifica el proceso de instalacion:

[Instalación de jekyll](https://jekyllrb.com/docs/installation/ubuntu/)

# Servir nuestro branch de gh-pages

Una vez clonado el repositorio de _adefesio_ y después de hacer `git checkout` al branch
gh-pages, ejecutamos el comando de jekyll que nos levanta el servidor web, y devuelve
un error:
```
~$ git clone https://github.com/recunchomaker/adefesio
Cloning into 'adefesio'...
remote: Enumerating objects: 159, done.
remote: Counting objects: 100% (159/159), done.
remote: Compressing objects: 100% (113/113), done.
remote: Total 159 (delta 61), reused 132 (delta 36), pack-reused 0
Receiving objects: 100% (159/159), 1.41 MiB | 2.82 MiB/s, done.
Resolving deltas: 100% (61/61), done.
~$ cd adefesio/
~/adefesio$ [master=] git checkout gh-pages 
Branch 'gh-pages' set up to track remote branch 'gh-pages' from 'origin'.
Switched to a new branch 'gh-pages'
~/adefesio$ [gh-pages=] bundle exec jekyll serve
Could not locate Gemfile or .bundle/ directory
```

En vez de crear una Gemfile de cero, creamos un blog temporal y copiaremos
su Gemfile a nuestra carpeta y volvemos a intentarlo.

```
~$ mkdir /tmp/borrame
~$ cd /tmp/borrame
/tmp/borrame$ jekyll new myblog
Running bundle install in /tmp/borrame/myblog...
  Bundler: The dependency tzinfo-data (>= 0) will be unused by any of the platforms Bundler is installing for. Bundler is installing for ruby but the dependency is only for x86-mingw32, x86-mswin32, x64-mingw32, java. To add those platforms to the bundle, run `bundle lock --add-platform x86-mingw32 x86-mswin32 x64-mingw32 java`.
  Bundler: Fetching gem metadata from https://rubygems.org/...........
  Bundler: Fetching gem metadata from https://rubygems.org/.
  Bundler: Resolving dependencies...
  Bundler: Using public_suffix 3.0.3
  (...)
  Bundler: Using minima 2.5.0
  Bundler: Bundle complete! 4 Gemfile dependencies, 29 gems now installed.
  Bundler: Use `bundle info [gemname]` to see where a bundled gem is installed.
New jekyll site installed in /tmp/borrame/myblog.
/tmp/borrame$ cd myblog/
/tmp/borrame/myblog$ ls
404.html  about.md  _config.yml  Gemfile  Gemfile.lock  index.md  _posts
/tmp/borrame/myblog$ cp Gemfile ~/adefesio
~$ cd ~/adefesio
~/adefesio$ [gh-pages %=] bundle exec jekyll serve
Configuration file: /tmp/adefesio/_config.yml
       Deprecation: The 'gems' configuration option has been renamed to 'plugins'. Please update your config file accordingly.
  Dependency Error: Yikes! It looks like you don't have jekyll-sitemap or one of its dependencies installed. In order to use Jekyll as currently configured, you'll need to install this gem. The full error message from Ruby is: 'cannot load such file -- jekyll-sitemap' If you run into trouble, you can find helpful resources at https://jekyllrb.com/help/! 
jekyll 3.8.5 | Error:  jekyll-sitemap
/tmp/adefesio✗ [gh-pages %=] 
```

Parece que nuestro config tiene un plugin que no está instalado localmente. Anadimos la linea "gem 'jekyll-sitemap'" a nuestro Gemfile. Nos dimos cuenta que en este fichero se especifica que para usar github pages se debe eliminar la entrada de gem 'jekyll' y descomentar gem 'gh-pages'. Lo hacemos también y ejecutamos `bundle update`

```
~/adefesio$ [gh-pages %=] bundle update
(...)
Fetching github-pages 193
Installing github-pages 193
Bundle updated!
Post-install message from dnsruby:
Installing dnsruby...
  For issues and source code: https://github.com/alexdalitz/dnsruby
  For general discussion (please tell us how you use dnsruby): https://groups.google.com/forum/#!forum/dnsruby
Post-install message from html-pipeline:
-------------------------------------------------
Thank you for installing html-pipeline!
You must bundle Filter gem dependencies.
See html-pipeline README.md for more details.
https://github.com/jch/html-pipeline#dependencies
-------------------------------------------------
~/adefesio✗ [gh-pages %=] bundle exec jekyll serve
Configuration file: /tmp/adefesio/_config.yml
       Deprecation: The 'gems' configuration option has been renamed to 'plugins'. Please update your config file accordingly.
            Source: /tmp/adefesio
       Destination: /tmp/adefesio/_site
 Incremental build: disabled. Enable with --incremental
      Generating... 
       Jekyll Feed: Generating feed for posts
                    done in 0.367 seconds.
 Auto-regeneration: enabled for '/tmp/adefesio'
    Server address: http://127.0.0.1:4000https://recunchomaker.github.io/adefesio//
  Server running... press ctrl-c to stop.
```

El servidor web se levanta, pero parece haber un error con las rutas y nos devuelve 404 en localhost:4000. Vamos a cambiar
el parametro "baseurl" por "/adefesio" en el `_config.yml`. Ahora sí que funciona, en la url localhost:4000/adefesio.

Sólo nos queda anadir un .gitignore con los temporales que genera el jekyll y subir el resultado.
```
$ cat .gitignore
_site/
```

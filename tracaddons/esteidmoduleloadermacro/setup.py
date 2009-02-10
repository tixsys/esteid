#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-

from setuptools import setup

setup(
    name = 'TracEstEIDModuleLoader',
    version = '1.0',
    packages = ['esteidmoduleloader'],
    author = 'Antti Andreimann',
    author_email = 'anttix@users.sourceforge.net',
    description = 'A Trac wiki macro to display Mozilla security module loaders',
    license = 'GPL',
    keywords = 'trac plugin macro esteid',
    url = 'https://id.eesti.ee/TracEstEIDModuleLoader',
    classifiers = [
        'Framework :: Trac',
    ],
    
    install_requires = [],

    entry_points = {
        'trac.plugins': [
            'esteidmoduleloader.macro = esteidmoduleloader.macro',
        ]
    },
)

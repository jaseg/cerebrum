from setuptools import setup
import os.path
import sys

ver = "0.0.1"

def read(filename):
    return open(os.path.join(os.path.dirname(__file__), filename)).read()

#This is a list of files to install, and where
#(relative to the 'root' dir, where setup.py is)
#You could be more specific.
files = ["pylibcerebrum/*"]

requires=['mako',
         ]

if sys.version_info < (3,3):
    requires.append('pylzma')
elif sys.version_info <(2,6):
    print("Oops, only python > 2.6 and >= 3.0 supported!")
    sys.exit()

setup(name = "pylibcerebrum",
    version = ver,
    description = "Python Library for cerebrum systems",
    license = "unknown",
    author = "jaseg",
    author_email = "jaseg@c-base.org",
    url = "https://github.com/jaseg/cerebrum",
    packages = ['pylibcerebrum',
                ],
    package_data = {'pylibcerebrum': ['pylibcerebrum']},
    scripts = ['runtests.py',
               'build.py',
               'generator.py'],
    entry_points="""
    """,
    install_requires=requires,
    zip_safe = True,
    classifiers = [
        'Development Status :: 4 - Beta',
        'Environment :: Console',
        'Environment :: Plugins',
        'Intended Audience :: Developers',
        'Intended Audience :: Information Technology',
        'Intended Audience :: Science/Research',
        'Programming Language :: Python :: 3.0',
        'Topic :: Scientific/Engineering :: Interface Engine/Protocol Translator',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Topic :: Home Automation',
        'Topic :: Software Development :: Embedded Systems',
        'Topic :: Software Development :: Libraries :: Application Frameworks',
        'Topic :: System :: Distributed Computing',
        'Topic :: System :: Hardware :: Hardware Drivers',
        'Topic :: System :: Networking'
    ],

    #package_data = {'package' : files },
    #scripts = [""], # None yet
    long_description = read('README.md'),
    # Dependencies
    #
    # Note: Those are proven to work, older versions might work, 
    # but have never been tested.
    #
    dependency_links = [],
)

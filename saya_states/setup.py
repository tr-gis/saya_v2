
## ! DO NOT MANUALLY INVOKE THIS setup.py, USE CATKIN INSTEAD

from distutils.core import setup
from catkin_pkg.python_setup import generate_distutils_setup

# fetch values from package.xml
setup_args = generate_distutils_setup(
    packages=['states'], # folder that contains the python libraies
    package_dir={'': 'src'}, # folder that contains the folder in which python library files are stored
)

setup(**setup_args)

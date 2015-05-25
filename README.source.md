# Git hooks

Git doesn't allow for automatically enabled hook scripts.
Because of this, you'll need to manually do the following:

  - remove the default `.git/hooks`, which only contains example scripts:
    ```rm -rf .git/hooks```
  - symlink the `hooks` directory in:
    ```ln -s ../hooks .git/hooks

The `pre-commit` script uses
[clang-format](http://clang.llvm.org/docs/ClangFormat.html), so make sure you
have that installed.

# Dependencies

  - cairo (with X support)
  - imlib2 (with X support)
  - pango
  - glib2
  - libX11
  - libXinerama
  - libXrandr
  - libXrender
  - libXcomposite
  - libXdamage
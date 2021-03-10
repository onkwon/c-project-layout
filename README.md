# C Project Layout

## Directories
```
.
├── assets
├── build
├── components
├── docs
├── examples
├── external
├── include
├── ports
├── projects
├── src
├── tests
└── tools
```

### `assets`

### `build`
A directory for build results, containing all object files.

It should not be considered part of the project source. It will be removed on
`clean`.

### `components`

### `docs`
Project documentation, possibly auto-generated.

### `examples`

### `external`
External dependencies including all the third party libraries.

This directory may be used for `git submodule`. Every submodule sources should
not be modified, keeping it as same as their upstream source. Modification to
the submodule should be made in a port layer.

The vendor provided SDK may also be here.

### `include`
Public headers.

### `ports`

### `projects`

### `src`
Source files of the project with subdirectories if needed.

Private headers may also be here.

It should not contain any submodules, but application related code only.

### `tests`

### `tools`

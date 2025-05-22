# Porter Tools

The `porter/` directory contains helper scripts for translating Eigen's C++
templates into a small C23 and Go interface.

* `scan_templates.py` – walk the `Eigen/` headers and collect template
  instantiations. ASTs are written to `porter/asts/` and the discovered
  mappings are stored in `porter/mapping.yaml`.
* `gen_headers.py` – using `mapping.yaml`, generate the C23 header
  `eigenc/include/ec_generated.h` which defines specialised matrix
  structures and operations.
* `build_all.sh` – convenience driver that regenerates headers and runs the
  test suite for both C and Go.

Run the following from the repository root to refresh generated files and
validate everything:

```bash
bash porter/build_all.sh
```

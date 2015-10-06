## tiff-multipage
Tool to merge multiple TIFF files into one.

### Requirements:
1. libtiff (tested on v4).

### Installation:
```bash
npm install tiff-multipage
```

### Usage example:
```javascript
let mp = require('tiff-multipage');

const OUTPUT_PATH = './output/OUT.TIFF';

let input_paths = ['./input/INPUT1.TIFF',
                   './input/INPUT2.TIFF',
                   './input/INPUT3.TIFF'];

// Async version
mp.joinAsync(OUTPUT_FILE_NAME, input_paths, function (err) {
    if (err) console.log(err);
});

// Sync version
try {
    mp.joinSync(OUTPUT_FILE_NAME, input_paths);
} catch (err) {
    console.log(err);
}
```

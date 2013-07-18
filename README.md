## tiff-multipage
Simple synchronous node.js module which utilize libtiff to make one multipage tiff from set of one-paged tiffs.

### Requirements:
1. libtiff (tested on v4).

### Installation:
```bash
npm install tiff-multipage
```

### Usage example:
```javascript
var tmp = require('tiff-multipage');
var pages = [];
var out = new tmp('/path/to/output.tiff', 'w');

pages.push(new tmp('/path/to/first.tiff', 'r'));
pages.push(new tmp('/path/to/second.tiff', 'r'));
pages.push(new tmp('/path/to/third.tiff', 'r'));
pages.push(new tmp('/path/to/nth.tiff', 'r'));

pages.forEach(out.pushAsPage.bind(out));
```

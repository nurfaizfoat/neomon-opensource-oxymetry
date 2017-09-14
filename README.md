# NEOMON - Neonate Monitor
![Alt text](https://i.imgur.com/zS2aS2z.jpg "NEOMON on Breadboard")
[![Open Source Love](https://badges.frapsoft.com/os/v1/open-source.svg?v=103)](https://github.com/ellerbrock/open-source-badges/)
**NEOMON** is an ESP8266 (Wireless MCU) and MAX30100 (IR Sensor) powered open source wearable-ready device. Its primary usage is to monitor newborn vital signs namely Heartbeat (BPM), SpO2 (Blood Oxygen Saturation) and also Body Temperature (°C/°F). These vital signs are the first line in pneumonia diagnosis and treatment in newborn babies.

Being an open source project, NEOMON is designed to be easy reproduce-able by anyone interested. Use cases are primarily targeted for poor, hard-to-reach communities and by low-skilled care providers. i.e Conflict Zone, War-torn countries. Its Bill of Materials (BOM) are also has to be kept reasonable (MYR300 @ USD$75).
## Overview

* [API Reference](https://github.com/d3/d3/blob/master/API.md)
* [Release Notes](https://github.com/d3/d3/releases)
* [Gallery](https://github.com/d3/d3/wiki/Gallery)
* [Examples](https://bl.ocks.org/mbostock)
* [Wiki](https://github.com/d3/d3/wiki)

## Installing

If you use npm, `npm install d3`. Otherwise, download the [latest release](https://github.com/d3/d3/releases/latest). The released bundle supports anonymous AMD, CommonJS, and vanilla environments. You can load directly from [d3js.org](https://d3js.org), [CDNJS](https://cdnjs.com/libraries/d3), or [unpkg](https://unpkg.com/d3/). For example:

```html
<script src="https://d3js.org/d3.v4.js"></script>
```

For the minified version:

```html
<script src="https://d3js.org/d3.v4.min.js"></script>
```

You can also use the standalone D3 microlibraries. For example, [d3-selection](https://github.com/d3/d3-selection):

```html
<script src="https://d3js.org/d3-selection.v1.js"></script>
```

D3 is written using [ES2015 modules](http://www.2ality.com/2014/09/es6-modules-final.html). Create a [custom bundle using Rollup](https://bl.ocks.org/mbostock/bb09af4c39c79cffcde4), Webpack, or your preferred bundler. To import D3 into an ES2015 application, either import specific symbols from specific D3 modules:

```js
import {scaleLinear} from "d3-scale";
```

Or import everything into a namespace (here, `d3`):

```js
import * as d3 from "d3";
```

In Node:

```js
var d3 = require("d3");
```

You can also require individual modules and combine them into a `d3` object using [Object.assign](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/assign):

```js
var d3 = Object.assign({}, require("d3-format"), require("d3-geo"), require("d3-geo-projection"));
```

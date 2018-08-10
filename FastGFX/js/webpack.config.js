module.exports = {
  context: __dirname,
  entry: ['babel-polyfill',"./runtime.js"],
  devtool: "source-map",
  module: {
  loaders: [
    {
      test: /\.jsx?$/,
      exclude: [/(node_modules)/,/evaluator\.js/],
      loader: 'babel', // 'babel-loader' is also a legal name to reference
      query: {
        presets: ['es2015']
      }
    }
  ]
},
  output: {
    path: __dirname,
      filename: "../runtime-js-compiled.js"//,
    // sourceMapFile: "index-compiled.js.map"
  },
  node: {
    fs: "empty"
  },
  plugins:[
  ]
}
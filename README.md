# nameless-reason

This repository holds an early version of the ReasonReact rewrite of [Nameless Language](https://github.com/nickretallack/nameless-language).  I switched to the new repository when I [upgraded to the newer way to write ReasonReact apps](https://reasonml.github.io/reason-react/docs/en/reasonreactcompat).

## Run Project

```sh
npm install
npm start
# in another tab
npm run webpack
```

After you see the webpack compilation succeed (the `npm run webpack` step), open up `src/index.html`

## Build for Production

```sh
npm run build
npm run webpack:production
```

This will replace the development artifact `build/Index.js` for an optimized version.

**To enable dead code elimination**, change `bsconfig.json`'s `package-specs` `module` from `"commonjs"` to `"es6"`. Then re-run the above 2 commands. This will allow Webpack to remove unused code.

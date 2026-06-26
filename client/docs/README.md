# Envoy Client docs

Envoy Client's docs are generated using [Sphinx](http://www.sphinx-doc.org),
and are published
[here](https://envoyclient.io/docs/envoy-client/latest/index.html).

## Generating docs locally

To generate the docs locally, run:

```bash
./docs/build.sh
```

The output can be then be found in `generated/docs`.

## Updating the Envoy Client website and docs

The docs website is automatically updated with the latest docs when a commit is
merged to main. This is done via the [publish script](./publish.sh).

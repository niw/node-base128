{
  "targets": [
    {
      "target_name": "base128",
      "sources": [
        "src/base128.cc"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}

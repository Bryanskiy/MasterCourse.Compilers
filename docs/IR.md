# Examples

## Example 1: fibonacci

C++ code:

```
int64_t fact(int32_t n) {
    int64_t res{1};
    for(int32_t i{2}; i <= n; ++i) {
        res *= i;
    }

    return res;
}
```

Jade IR:

```
fn fact(v0: i32) -> i64 {
    bb0: {
        v1: i64 = const 1_64;
        v2: i32 = const 2_32;
        goto -> bb1;
    }

    bb1: {
        v3: i32 = phi [v2, bb0], [b7, bb3];
        v4: i1 = v3 <= v0;
        if (v4, bb2, bb3);
    }

    bb2: { // false branch
        v5: i64 = phi [v1, bb0], [v10, bb3];
        ret v5;
    }

    bb3: { // true branch
        v6: i32 = phi [v2, bb0], [v7, bb3]
        v7: i32 = v6 + const 1_i32;
        v8: i64 = v7 as i64;
        v9: i64 = phi [v1, bb0], [v10, bb3];
        v10: i64 = v8 * v9;
        goto -> bb1;
    }
}
```

# TODO:

1. LVALUE: support of agregates(arrays, classes)
2. How to process calls? (in particular exeptions from calls)
3. Should allocas been used?

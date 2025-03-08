# TRNG-RDRAND-BM
A kind of simple implementation of RDRAND (likewise, AMD or Intel doesn't matter) uses 8 tests to determine whether it's TRNG or not. 

## Note
This doesn't mean it's correct, though if it fails a lot of tests, it's probably not random or a pseudocode RNG. If it passes 5 out of the 8 tests, I would say it's good to use in production. 

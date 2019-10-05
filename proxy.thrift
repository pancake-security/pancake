service pancake_thrift {
    string get(1: string key),
    string put(1: string key, 2: string value),
    list<string> get_batch(1: list<string> keys),
    string put_batch(1: list<string> keys, 2: list <string> values)
}

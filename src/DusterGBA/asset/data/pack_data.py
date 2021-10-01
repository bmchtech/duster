import msgpack
import json
import sys

INF = sys.argv[1]
OUF = sys.argv[2]

with open(INF) as inf:
    data = json.loads(inf.read())
    print('data:', data)
    with open(OUF, 'wb') as ouf:
        bin_data = msgpack.packb(data, use_bin_type=True)
        print(bin_data)
        ouf.write(bin_data)

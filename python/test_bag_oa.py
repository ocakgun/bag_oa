from cybagoa import PyOAWriter

writer = PyOAWriter('./cds.lib', 'AAAFOO', 'testpyoa', 'layout')
writer.add_purpose('pin', 251)

writer.create_rect(('M1', 'drawing'), [[0.0, 0.0], [0.2, 0.1]],
                   arr_nx=3, arr_ny=2, arr_spx=0.25, arr_spy=0.2)

writer.create_pin('foo', 'foo1', 'foo:', ('M2', 'pin'),
                  [[0.5, 0.5], [0.7, 0.6]])

writer.create_via('M2_M1', (0.6, 0.6), 'R0', 2, 3, 0.06, 0.06,
                  [0.04, 0.05, 0.0, 0.0], [0.0, 0.0, 0.06, 0.05],
                  arr_nx=4, arr_spx=0.5)

writer.close()

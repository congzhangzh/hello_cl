default: devices platforms FORCE

devices: FORCE
	g++ devices.c -o devices -lOpenCL &&  ( echo --begin-- devices ; ./devices ; echo --end-- devices )

platforms: FORCE
	g++ platforms.c -o platforms -lOpenCL &&  ( echo --begin-- platform ; ./platforms ; echo --end-- platform )

FORCE: ;
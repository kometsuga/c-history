@echo off

del *.bin /S
del *.exe /S
del *.sdf /S
del *.pdb /S
del *.psess /S
del *.ilk /S
del *Log*.txt /S
del *���O*.txt /S
del *�낮*.txt /S
del *���|�[�g*.* /S

rmdir Release /Q /S
rmdir Debug /Q /S
rmdir test\Release /Q /S
rmdir test\Debug /Q /S
rmdir test\x64 /Q /S
rmdir ipch /Q /S
rmdir x64 /Q /S


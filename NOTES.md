## Design Notes

Everything that is in the `emit/` directory should follow the same rules as the original MachSuite. That means that if you generated a certain scale, you could copy the whole directory somewhere else and use it just like you would use MachSuite. We'll have to have a little hack to export a Makefile from SCons.


At some point, we're going to need scalable (auto-generated) HLS files for everything. read: Brandon.

Each scale should have its own emit directory. (`emit/scaleN/...`)

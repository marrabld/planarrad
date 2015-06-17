

noinst_SCRIPTS = \
	test_script_free \
	skytool_test_script_free \
	surftool_test_script_free \
	slabtool_test_script_free




test_script_free: test_script.in Makefile
	cp $(srcdir)/test_script.in test_script_free
	chmod +x test_script_free

skytool_test_script_free: skytool_test_script.in Makefile
	cp $(srcdir)/skytool_test_script.in skytool_test_script_free
	chmod +x skytool_test_script_free
	cp -R $(srcdir)/skytool_test01 ./
	cp -R $(srcdir)/skytool_test02 ./
	cp -R $(srcdir)/skytool_test03 ./
	cp -R $(srcdir)/skytool_test04 ./

surftool_test_script_free: surftool_test_script.in Makefile
	cp $(srcdir)/surftool_test_script.in surftool_test_script_free
	chmod +x surftool_test_script_free
	cp -R $(srcdir)/surftool_test01 ./
	cp -R $(srcdir)/surftool_test02 ./

slabtool_test_script_free: slabtool_test_script.in Makefile
	cp $(srcdir)/slabtool_test_script.in slabtool_test_script_free
	chmod +x slabtool_test_script_free
	cp -R $(srcdir)/slabtool_test01 ./







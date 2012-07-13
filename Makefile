cleanall: clean_master clean_slave cleanhex_master cleanhex_slave cleandep_master cleandep_slave

cleanhex_master:
	rm -f build-master*.hex

cleanhex_slave:
	rm -f build-slave*.hex

cleandep_master:
	rm -f build-master/*.o.d

cleandep_slave:
	rm -f build-slave/*.o.d

clean_master:
	rm -f build-master/*.o build-master/*.map build-master/*.elf build-master/*.eep build-master/*.lss
	
clean_slave:
	rm -f build-slave/*.o build-slave/*.map build-slave/*.elf build-slave/*.eep build-slave/*.lss
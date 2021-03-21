function(target_precompiled_header project_target pch_source project_sources)
if(MSVC)
	get_filename_component(pch_basename ${pch_source} NAME_WE)
	set(pch_header "${pch_basename}.h")
	
	set_target_properties(${project_target} PROPERTIES COMPILE_FLAGS "/Yu${pch_header}")
	set_source_files_properties(${pch_source} PROPERTIES COMPILE_FLAGS "/Yc${pch_header}")
endif(MSVC)
endfunction()
# group source on visual project
function(setup_project_group project_source current_dir)
	foreach(source IN LISTS project_source)
		# get source path
		get_filename_component(source_path ${source} PATH)
		
		# get source relative path
		string(REPLACE "${current_dir}/./" "" source_relative ${source_path})
		
		if(MSVC OR XCODE)
			# get group name
			string(REPLACE "/" "\\" group_name ${source_relative})
		else()
			set(group_name ${source_relative})
		endif()
		
		if("${source_relative}" STREQUAL "${source_path}")
			# no need group because source is in $current_dir
		else()
			# setup project group
			source_group("${group_name}" FILES "${source}")
		endif()
	endforeach()
endfunction()

function(add_source_group project_source group_name)
	foreach(source IN LISTS project_source)
		# setup project group
		source_group("${group_name}" FILES "${source}")
	endforeach()
endfunction()
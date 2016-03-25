# threadpool library import helper
function(FindThreadPool package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../ThreadPool
                           ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
endfunction(FindThreadPool)

FindThreadPool(ThreadPool)

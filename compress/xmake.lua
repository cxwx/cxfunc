set_project("cxfileio")
set_version("0.5.0")

set_languages("c++17")

-- Boost
add_requires("boost", {configs = {iostreams = true, zlib = true, bzip2 = true, lzma = true, zstd = true}})

-- ROOT dictionary generation rule
rule("root_dict")
    set_extensions(".hh")
    before_buildcmd_file(function (target, batchcmds, sourcefile, opt)
        local header = path.filename(sourcefile)
        local dict_name = "G__" .. target:name() .. ".cxx"
        local dict_path = path.join(target:objectdir(), dict_name)
        local pcm_path = path.join(target:objectdir(), "lib" .. target:name() .. "_rdict.pcm")
        local rootmap_path = path.join(target:objectdir(), "lib" .. target:name() .. ".rootmap")
        local root_incdir = target:data("root_incdir")

        batchcmds:show_progress(opt.progress, "${color.build.object}generating ROOT dict %s", header)
        batchcmds:mkdir(path.directory(dict_path))
        batchcmds:vrunv("rootcling", {
            "-f", dict_path,
            "-rmf", rootmap_path,
            "-rml", "lib" .. target:name() .. ".so",
            "-s", pcm_path,
            "-I", root_incdir,
            "-I", os.projectdir(),
            header,
            "LinkDef.h"
        })

        batchcmds:add_depfiles(sourcefile)
        batchcmds:set_depmtime(os.mtime(dict_path))
        batchcmds:set_depcache(target:dependfile(dict_path))
    end)

-- Shared library
target("cxfileio")
    set_kind("shared")

    add_files("cxFileIO.cc")
    add_headerfiles("cxFileIO.hh", "LinkDef.h")

    -- ROOT dictionary
    add_rules("root_dict")
    add_files("cxFileIO.hh")

    -- Dependencies
    add_packages("boost")

    -- Detect ROOT in on_load
    on_load(function (target)
        local root_incdir = os.iorun("root-config --incdir"):trim()
        local root_libdir = os.iorun("root-config --libdir"):trim()

        target:data_set("root_incdir", root_incdir)

        target:add("includedirs", root_incdir)
        target:add("linkdirs", root_libdir)
        target:add("syslinks",
            "Core", "RIO", "Net", "Imt", "Hist", "Graf", "Graf3d",
            "Gpad", "Tree", "Rint", "Postscript", "Matrix", "Physics",
            "MathCore", "Thread", "MultiProc", "ROOTVecOps")

        if is_plat("macosx") then
            target:add("shflags", "-undefined dynamic_lookup")
        end
    end)

    -- Install
    set_installdir(path.join(os.getenv("HOME"), "software/install"))
    add_installfiles("*.hh", {prefixdir = "include/cxfunc"})

-- Test executable
target("cxfileio_test")
    set_kind("binary")
    set_default(false, {install = false})

    add_files("test/main.cpp")
    add_deps("cxfileio")

    add_packages("boost")

    on_load(function (target)
        local root_incdir = os.iorun("root-config --incdir"):trim()
        target:add("includedirs", root_incdir)
    end)

project(
  'r2garlic', 'c',
  version: '0.1.0',
  license: 'MIT',
  default_options: [
    'c_std=c11',
    'warning_level=2',
    'werror=false',
    'buildtype=debugoptimized',
  ],
)

cc = meson.get_compiler('c')

add_project_arguments(
  [
    '-D_GNU_SOURCE',
    '-DGARLIC_NO_MAIN',
  ],
  language: 'c',
)

add_project_arguments(
  cc.get_supported_arguments([
    '-Wno-unused-variable',
    '-Wno-unused-function',
    '-Wno-unused-parameter',
    '-Wno-unused-but-set-variable',
    '-Wno-implicit-function-declaration',
    '-Wno-incompatible-pointer-types',
    '-Wno-misleading-indentation',
    '-Wno-format',
    '-Wno-sign-compare',
    '-Wno-cast-function-type-mismatch',
  ]),
  language: 'c',
)

r_core_dep = dependency('r_core')
threads_dep = dependency('threads')

garlic_src = 'garlic/src'

garlic_inc = include_directories(
  garlic_src,
  garlic_src / 'common',
  garlic_src / 'libs/memory',
  garlic_src / 'libs/hashmap',
  garlic_src / 'libs/list',
  garlic_src / 'libs/bitset',
  garlic_src / 'libs/queue',
  garlic_src / 'libs/str',
  garlic_src / 'libs/zip',
  garlic_src / 'libs/threadpool',
  garlic_src / 'libs/trie',
  garlic_src / 'parser/class',
  garlic_src / 'parser/dex',
  garlic_src / 'parser/pe',
  garlic_src / 'parser/reader',
  garlic_src / 'dalvik',
  garlic_src / 'decompiler',
  garlic_src / 'decompiler/transformer',
  garlic_src / 'jvm',
  garlic_src / 'jar',
  garlic_src / 'apk',
  'src',
)

garlic_sources = files(
  'garlic/src/apk/apk.c',
  'garlic/src/dalvik/dex_annotation.c',
  'garlic/src/dalvik/dex_class.c',
  'garlic/src/dalvik/dex_decompile.c',
  'garlic/src/dalvik/dex_descriptor.c',
  'garlic/src/dalvik/dex_dump.c',
  'garlic/src/dalvik/dex_exception.c',
  'garlic/src/dalvik/dex_expression_builder.c',
  'garlic/src/dalvik/dex_ins.c',
  'garlic/src/dalvik/dex_ins_action.c',
  'garlic/src/dalvik/dex_lambda.c',
  'garlic/src/dalvik/dex_meta_helper.c',
  'garlic/src/dalvik/dex_method.c',
  'garlic/src/dalvik/dex_optimizer.c',
  'garlic/src/dalvik/dex_pre_optimizer.c',
  'garlic/src/dalvik/dex_simulator.c',
  'garlic/src/dalvik/dex_smali.c',
  'garlic/src/dalvik/dex_type_analyse.c',
  'garlic/src/dalvik/dex_writter.c',
  'garlic/src/decompiler/control_flow.c',
  'garlic/src/decompiler/descriptor.c',
  'garlic/src/decompiler/dominator_tree.c',
  'garlic/src/decompiler/exception.c',
  'garlic/src/decompiler/expression.c',
  'garlic/src/decompiler/expression_analyse.c',
  'garlic/src/decompiler/expression_array.c',
  'garlic/src/decompiler/expression_assert.c',
  'garlic/src/decompiler/expression_assign.c',
  'garlic/src/decompiler/expression_branches.c',
  'garlic/src/decompiler/expression_chain.c',
  'garlic/src/decompiler/expression_copy_propgation.c',
  'garlic/src/decompiler/expression_enum.c',
  'garlic/src/decompiler/expression_exception.c',
  'garlic/src/decompiler/expression_goto.c',
  'garlic/src/decompiler/expression_if.c',
  'garlic/src/decompiler/expression_inline.c',
  'garlic/src/decompiler/expression_inner_class.c',
  'garlic/src/decompiler/expression_local_variable.c',
  'garlic/src/decompiler/expression_logical.c',
  'garlic/src/decompiler/expression_loop.c',
  'garlic/src/decompiler/expression_loop_type.c',
  'garlic/src/decompiler/expression_new.c',
  'garlic/src/decompiler/expression_node.c',
  'garlic/src/decompiler/expression_node_param.c',
  'garlic/src/decompiler/expression_remove_useless.c',
  'garlic/src/decompiler/expression_return.c',
  'garlic/src/decompiler/expression_synchronized.c',
  'garlic/src/decompiler/expression_ternary.c',
  'garlic/src/decompiler/expression_visitor.c',
  'garlic/src/decompiler/expression_writter.c',
  'garlic/src/decompiler/field.c',
  'garlic/src/decompiler/instruction.c',
  'garlic/src/decompiler/klass.c',
  'garlic/src/decompiler/method.c',
  'garlic/src/decompiler/scc.c',
  'garlic/src/decompiler/signature.c',
  'garlic/src/decompiler/ssa.c',
  'garlic/src/decompiler/stack.c',
  'garlic/src/decompiler/transformer/anonymous.c',
  'garlic/src/decompiler/transformer/array_load.c',
  'garlic/src/decompiler/transformer/array_store.c',
  'garlic/src/decompiler/transformer/arraylength.c',
  'garlic/src/decompiler/transformer/assert.c',
  'garlic/src/decompiler/transformer/assignment.c',
  'garlic/src/decompiler/transformer/assignment_chain.c',
  'garlic/src/decompiler/transformer/athrow.c',
  'garlic/src/decompiler/transformer/cast.c',
  'garlic/src/decompiler/transformer/const.c',
  'garlic/src/decompiler/transformer/declaration.c',
  'garlic/src/decompiler/transformer/define_stack_var.c',
  'garlic/src/decompiler/transformer/enum.c',
  'garlic/src/decompiler/transformer/get_field.c',
  'garlic/src/decompiler/transformer/get_static.c',
  'garlic/src/decompiler/transformer/goto.c',
  'garlic/src/decompiler/transformer/if.c',
  'garlic/src/decompiler/transformer/iinc.c',
  'garlic/src/decompiler/transformer/initialize.c',
  'garlic/src/decompiler/transformer/instanceof.c',
  'garlic/src/decompiler/transformer/invoke.c',
  'garlic/src/decompiler/transformer/invokedynamic.c',
  'garlic/src/decompiler/transformer/invokeinterface.c',
  'garlic/src/decompiler/transformer/invokespecial.c',
  'garlic/src/decompiler/transformer/invokestatic.c',
  'garlic/src/decompiler/transformer/invokevirtual.c',
  'garlic/src/decompiler/transformer/lambda.c',
  'garlic/src/decompiler/transformer/local_variable.c',
  'garlic/src/decompiler/transformer/logic_not.c',
  'garlic/src/decompiler/transformer/loop.c',
  'garlic/src/decompiler/transformer/lvalue.c',
  'garlic/src/decompiler/transformer/monitorenter.c',
  'garlic/src/decompiler/transformer/monitorexit.c',
  'garlic/src/decompiler/transformer/new_array.c',
  'garlic/src/decompiler/transformer/operator.c',
  'garlic/src/decompiler/transformer/put_field.c',
  'garlic/src/decompiler/transformer/put_static.c',
  'garlic/src/decompiler/transformer/return.c',
  'garlic/src/decompiler/transformer/single_list.c',
  'garlic/src/decompiler/transformer/single_operator.c',
  'garlic/src/decompiler/transformer/stack_value.c',
  'garlic/src/decompiler/transformer/stack_var.c',
  'garlic/src/decompiler/transformer/store.c',
  'garlic/src/decompiler/transformer/str_concat.c',
  'garlic/src/decompiler/transformer/switch.c',
  'garlic/src/decompiler/transformer/ternary.c',
  'garlic/src/decompiler/transformer/transformer.c',
  'garlic/src/decompiler/transformer/uninitialize.c',
  'garlic/src/jar/jar.c',
  'garlic/src/jvm/jvm_annotation.c',
  'garlic/src/jvm/jvm_class.c',
  'garlic/src/jvm/jvm_decompile.c',
  'garlic/src/jvm/jvm_descriptor.c',
  'garlic/src/jvm/jvm_exception.c',
  'garlic/src/jvm/jvm_expression_builder.c',
  'garlic/src/jvm/jvm_ins.c',
  'garlic/src/jvm/jvm_ins_action.c',
  'garlic/src/jvm/jvm_lambda.c',
  'garlic/src/jvm/jvm_method.c',
  'garlic/src/jvm/jvm_optimizer.c',
  'garlic/src/jvm/jvm_simulator.c',
  'garlic/src/jvm/jvm_type_analyse.c',
  'garlic/src/libs/bitset/bitset.c',
  'garlic/src/libs/hashmap/hashmap.c',
  'garlic/src/libs/hashmap/hashmap_tools.c',
  'garlic/src/libs/list/list.c',
  'garlic/src/libs/memory/mem_pool.c',
  'garlic/src/libs/queue/queue.c',
  'garlic/src/libs/str/str.c',
  'garlic/src/libs/threadpool/threadpool.c',
  'garlic/src/libs/trie/trie_tree.c',
  'garlic/src/libs/zip/zip.c',
  'garlic/src/parser/class/class_tools.c',
  'garlic/src/parser/class/metadata.c',
  'garlic/src/parser/class/opcode.c',
  'garlic/src/parser/class/writter.c',
  'garlic/src/parser/dex/metadata.c',
  'garlic/src/parser/dex/opcode.c',
  'garlic/src/parser/pe/metadata.c',
)

libgarlic = static_library(
  'garlic',
  garlic_sources,
  include_directories: garlic_inc,
  dependencies: threads_dep,
  pic: true,
)

r2_plugindir = get_option('r2_plugindir')
if r2_plugindir == ''
  r2 = find_program('r2', required: false)
  if r2.found()
    r2_user_plugins = run_command(r2, '-H', 'R2_USER_PLUGINS', check: false)
    if r2_user_plugins.returncode() == 0
      r2_plugindir = r2_user_plugins.stdout().strip()
    endif
  endif
endif

if r2_plugindir == ''
  r2_plugindir = join_paths(get_option('libdir'), 'radare2', 'plugins')
  warning('r2 was not found; installing the plugin under ' + r2_plugindir)
endif

shared_library(
  'core_r2garlic',
  files(
    'src/r2garlic.c',
    'src/memstream.c',
  ),
  include_directories: garlic_inc,
  dependencies: [
    r_core_dep,
    threads_dep,
  ],
  link_with: libgarlic,
  name_prefix: '',
  install: true,
  install_dir: r2_plugindir,
)

message('r2garlic plugin install path: ' + r2_plugindir)

{
  'targets': [
    {
      'target_name': 'rcib',
      'win_delay_load_hook': 'false',
      'conditions':[
        [
          'OS=="win"', {
            'sources': [
              'src/rcib.cc',
              'src/rcib_object.cc',
              'src/rcib/at_exist.cc',
              'src/rcib/lazy_instance.cc',
              'src/rcib/MessageLoop.cc',
              'src/rcib/MessagePumpDefault.cc',
              'src/rcib/PendingTask.cc',
              'src/rcib/ref_counted.cc',
              'src/rcib/Thread.cc',
              'src/rcib/util_tools.cc',
              'src/rcib/WeakPtr.cc',
              'src/rcib/Event/WaitableEvent.cc',
              'src/rcib/time/time.cc',
              # headers to make for a more pleasant IDE experience
              'src/rcib.h',
              'src/rcib/aligned_memory.h',
              'src/rcib/at_exist.h',
              'src/rcib/FastDelegate.h',
              'src/rcib/FastDelegateImpl.h',
              'src/rcib/lazy_instance.h',
              'src/rcib/macros.h',
              'src/rcib/MessageLoop.h',
              'src/rcib/MessagePump.h',
              'src/rcib/MessagePumpDefault.h',
              'src/rcib/observer_list.h',
              'src/rcib/PendingTask.h',
              'src/rcib/ref_counted.h',
              'src/rcib/Thread.h',
              'src/rcib/thread_local.h',
              'src/rcib/util_tools.h',
              'src/rcib/WeakPtr.h',
              'src/rcib/WrapperObj.h',
              'src/rcib/Event/WaitableEvent.h',
              'src/rcib/time/time.h'
            ],
            'libraries':[],
            'cflags':[]
          }],
        [
          'OS=="linux"', {
            'sources': [
              'src/rcib.cc',
              'src/rcib_object.cc',
              'src/rcib/at_exist.cc',
              'src/rcib/lazy_instance.cc',
              'src/rcib/MessageLoop.cc',
              'src/rcib/MessagePumpDefault.cc',
              'src/rcib/PendingTask.cc',
              'src/rcib/ref_counted.cc',
              'src/rcib/Thread.cc',
              'src/rcib/util_tools.cc',
              'src/rcib/WeakPtr.cc',
              'src/rcib/Event/WaitableEvent.cc',
              'src/rcib/time/time.cc',
              # headers to make for a more pleasant IDE experience
              'src/rcib.h',
              'src/rcib/aligned_memory.h',
              'src/rcib/at_exist.h',
              'src/rcib/FastDelegate.h',
              'src/rcib/FastDelegateImpl.h',
              'src/rcib/lazy_instance.h',
              'src/rcib/macros.h',
              'src/rcib/MessageLoop.h',
              'src/rcib/MessagePump.h',
              'src/rcib/MessagePumpDefault.h',
              'src/rcib/observer_list.h',
              'src/rcib/PendingTask.h',
              'src/rcib/ref_counted.h',
              'src/rcib/Thread.h',
              'src/rcib/thread_local.h',
              'src/rcib/util_tools.h',
              'src/rcib/WeakPtr.h',
              'src/rcib/WrapperObj.h',
              'src/rcib/Event/WaitableEvent.h',
              'src/rcib/time/time.h'
            ],
            'libraries':[],
            'cflags':[]
          }]
      ]
    }
  ]
}

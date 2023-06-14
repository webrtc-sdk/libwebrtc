import fileinput
from conan import ConanFile
from conan.tools.files import chdir, mkdir, save, load
import pathlib
import os


class LibWebRTCConan(ConanFile):
    name = 'libwebrtc'
    version = '0.0.1'
    settings = 'os', 'build_type', 'arch'
    generators = [ "CMakeDeps" ]

    depot_tools_repository = 'https://chromium.googlesource.com/chromium/tools/depot_tools.git'
    depot_tools_release = 'a104c01252f54997e672490e7ab6cb7e15295a98'
    depot_tools_dir = 'depot_tools'
    
    webrtc_release = 'm104'
    webrtc_dir = 'webrtc'


    def setup_depot_tools(self):
        try:
            with chdir(self, self.depot_tools_dir):
                print('-- Depot tools directory already exists')
        except:
            print('-- Setting up depot tools')

            self.run(f'git clone {self.depot_tools_repository} {self.depot_tools_dir}')

            # gclient has to be called twice, on windows if it's called once, 
            # after a switch to specified branch there are issue with python paths
            with chdir(self, self.depot_tools_dir):
                os.environ['DEPOT_TOOLS_WIN_TOOLCHAIN'] = '0'

                self.run('gclient')

                self.run(f'git checkout {self.depot_tools_release}')

                os.environ['DEPOT_TOOLS_UPDATE'] = '0'

                self.run('gclient')


    def set_depot_tools_environment_variables(self):
        current_path = pathlib.Path(__file__).parent.resolve()
        depot_tools_path = os.path.join(os.path.sep, str(current_path), 'build','externals', self.depot_tools_dir)
        os.environ['PATH'] += os.pathsep + depot_tools_path
        os.environ['DEPOT_TOOLS_WIN_TOOLCHAIN'] = '0'
        os.environ['DEPOT_TOOLS_UPDATE'] = '0'


    def create_gclient_configuration(self):
        gclientFileContent = """\
solutions = [
  {{
    "name"        : '{}',
    "url"         : 'https://github.com/webrtc-sdk/webrtc.git@{}_release',
    "deps_file"   : 'DEPS',
    "managed"     : False,
    "custom_deps" : {{}},
    "custom_vars": {{}},
  }},
]
""".format(self.webrtc_dir, self.webrtc_release)

        with open('.gclient', 'w') as file:
            file.writelines(gclientFileContent)


    def setup_webrtc(self):
        try:
           with chdir(self, self.webrtc_dir):
               return
        except:
           print('-- Setting up WebRTC')

        self.create_gclient_configuration()
        
        self.run('gclient sync')


    def source(self):
        mkdir(self, "build")
        with chdir(self, 'build'):
            mkdir(self, "externals")
            with chdir(self, 'externals'):
                self.setup_depot_tools()

                self.set_depot_tools_environment_variables()

                self.setup_webrtc()


    def setup_webrtc_on_linux(self):
        self.run('sed -i "s/} snapcraft/} /gi" build/install-build-deps.sh')
        self.run('build/install-build-deps.sh --no-prompt')

        if self.settings.arch == 'x86_64':
            self.run('python3 build/linux/sysroot_scripts/install-sysroot.py --arch=amd64')
        elif self.settings.arch == 'armv8':
            self.run('python3 build/linux/sysroot_scripts/install-sysroot.py --arch=arm64')
        else:
            raise 'Unsupported Linux distribution'


    def configure_webrtc(self):
        try:
            load(self, 'webrtc_configuration_lock')
            return
        except:
           print('-- Configure WebRTC')

        with chdir(self, self.webrtc_dir):
            if self.settings.os == 'Linux':
                self.setup_webrtc_on_linux()

            with fileinput.FileInput('BUILD.gn', inplace=True) as file:
                for line in file:
                    print(line.replace("deps = [ \":webrtc\" ]", "deps = [ \":webrtc\", \"../../../libwebrtc\" ]"), end='')

        save(self, 'webrtc_configuration_lock', '')


    def gn_args(self):
        args = []

        args.append('is_clang=true')
        args.append('is_debug=false')
        args.append('rtc_include_tests=false')
        args.append('is_component_build=false')
        args.append('rtc_use_h264=true')
        args.append('ffmpeg_branding=\"Chrome\"')

        if self.settings.os == 'Windows':
            args.append('target_os=\"win\"')
            args.append('libwebrtc_desktop_capture=true')

        if self.settings.os == 'Linux':
            args.append('target_os=\"linux\"')

            if self.settings.arch == 'x86_64':
                args.append('target_cpu=\"x64\"')
            elif self.settings.arch == 'armv8':
                args.append('target_cpu=\"arm64\"')
            else:
                raise 'Unsupported Linux distribution'

        return " ".join(args)


    def build_webrtc(self):
        with chdir(self, 'src'):
            gn_args = self.gn_args()

            self.run(f'gn gen out --args="{gn_args}"')

            self.run("ninja -C out")

            self.run("ninja -C out libwebrtc")


    def build(self):
        with chdir(self, 'externals'):
            self.set_depot_tools_environment_variables()

            self.configure_webrtc()

            self.build_webrtc()

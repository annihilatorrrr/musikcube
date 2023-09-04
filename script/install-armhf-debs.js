const { promisify } = require('util');
const exec = promisify(require('child_process').exec);
const fs = require('fs');
const path = require('path');

/**
 * This script will download and extract all transitive dependencies
 * for the packages specified by PACKAGE_NAMES. This is used to populate
 * a cross-compile sysroot with the libraries required to compile the
 * app itself.
 */
const PACKAGE_NAMES = [
  'libev-dev',
  'libev4',
  'libncurses-dev',
  'libncursesw6',
  'libopus-dev',
  'libopus0',
  'libsystemd-dev',
  'libtinfo6',
  'libvorbis-dev',
  'libvorbis0a',
  'libvorbisenc2',
  'zlib1g-dev',
];

const EXCLUDE = [
  'gcc-8-base',
  'gcc-10-base',
  'gcc-13-base',
  'libc-dev-bin',
  'libc6',
  'libcrypt1',
  'libgcc-s1',
  'libgcc1',
  'linux-libc-dev',
];

const CLEANUP_FILES = [
  'control.tar.xz',
  'control.tar.zst',
  'data.tar.xz',
  'data.tar.zst',
  'debian-binary',
];

const getPackageDependencies = async (packageName) => {
  console.log('scanning', packageName);
  const rawOutput = (
    await exec(
      `apt-cache depends --recurse --no-recommends --no-suggests --no-conflicts --no-breaks --no-replaces --no-enhances ${packageName}`
    )
  ).stdout.split('\n');
  const packages = rawOutput
    .filter((e) => e.indexOf('Depends:') >= 0)
    .map((e) => e.replace('Depends:', '').trim())
    .filter(
      (e) => !(e.startsWith('Pre ') || e.startsWith('<') || e.startsWith('|'))
    );
  return packages;
};

const getPackageDownloadUrls = async (packages) => {
  const rawOutput = (
    await exec(`apt download --print-uris ${packages.join(' ')}`)
  ).stdout.split('\n');
  const downloadUrls = rawOutput
    .filter((e) => e.trim().length > 0)
    .map((e) => e.split(' ')[0].trim().replace(/'/g, ''));
  return downloadUrls;
};

const downloadAndExtract = async (downloadUrls) => {
  for (let i = 0; i < downloadUrls.length; i++) {
    const fn = decodeURIComponent(downloadUrls[i].split('/').pop());
    console.log('processing', downloadUrls[i]);
    await exec(`wget ${downloadUrls[i]}`);
    await exec(`ar x ./${fn}`);
    if (fs.existsSync('data.tar.zst')) {
      await exec(`tar --use-compress-program=unzstd -xvf data.tar.zst`);
    } else if (fs.existsSync('data.tar.xz')) {
      await exec(`tar -xvf data.tar.xz`);
    } else {
      console.error('unknown file type');
      process.exit(-1);
    }
    for (let j = 0; j < CLEANUP_FILES.length; j++) {
      if (fs.existsSync(CLEANUP_FILES[j])) {
        await exec(`rm ${CLEANUP_FILES[j]}`);
      }
    }
  }
};

const convertAbsoluteToRelativeSymlinks = async () => {
  const root = __dirname;
  const symlinks = (await exec('find . -type l')).stdout
    .split('\n')
    .filter((e) => e.length > 0);
  for (let i = 0; i < symlinks.length; i++) {
    const dest = fs.readlinkSync(symlinks[i]);
    if (dest[0] === '/') {
      const absolute = path.join(root, dest);
      const relative = path.relative(symlinks[i], absolute);
      console.log(`relinking\n * from: ${absolute}\n * to: ${relative}`);
      // fs.unlinkSync(symslinks[i]);
      // fs.link(relative, symlinks[i]);
    }
  }
};

const rmDebs = async () => {
  try {
    console.log('cleaning up downloads');
    await exec('rm *.deb');
  } catch (e) {
    /* nothing */
  }
};

const main = async () => {
  await rmDebs();
  let dependencies = [];
  for (let i = 0; i < PACKAGE_NAMES.length; i++) {
    dependencies = [
      ...dependencies,
      ...(await getPackageDependencies(PACKAGE_NAMES[i])),
    ];
  }
  const deduped = new Set([...PACKAGE_NAMES, ...dependencies]);
  for (let i = 0; i < EXCLUDE.length; i++) {
    deduped.delete(EXCLUDE[i]);
  }
  console.log('resolved transitive dependencies:', Array.from(deduped).sort());
  const downloadUrls = await getPackageDownloadUrls(Array.from(deduped));
  console.log('download urls:', downloadUrls);
  await downloadAndExtract(downloadUrls);
  await convertAbsoluteToRelativeSymlinks();
  await rmDebs();
  await exec('tar cvf sysroot.tar .');
};

main();

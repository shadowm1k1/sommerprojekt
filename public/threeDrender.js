import WebGL from 'three/addons/capabilities/WebGL.js'; if ( WebGL.isWebGLAvailable() ) { } else { const warning = WebGL.getWebGLErrorMessage(); document.getElementById( 'container' ).appendChild( warning ); }
import * as THREE from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';


const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera( 75, window.innerWidth / window.innerHeight, 0.1, 1000 );

const prc = document.getElementById('threeDView');

var renderer = new THREE.WebGLRenderer();
renderer.setSize(prc.offsetWidth,prc.offsetHeight );
renderer.setClearColor(0xBBBBBB);
prc.appendChild( renderer.domElement );

const controls = new OrbitControls( camera, renderer.domElement );

//light to see obj better
const directionalLight = new THREE.DirectionalLight( 0xffffff, 2.0 );
scene.add( directionalLight );

const light = new THREE.DirectionalLight( 0xffffff, 0.5 );

  light.position.set(0, 10, 0);
scene.add( light);

const dilight = new THREE.DirectionalLight( 0xffffff, 5);
dilight.position.set(-10,20, -10);
scene.add(dilight);

const delight = new THREE.DirectionalLight(0xffffff, 5);
delight.position.set(10,10, 10);
scene.add(delight);

const belowlight = new THREE.DirectionalLight( 0xffffff, 3 );
belowlight.position.set(0,-10,0);
scene.add(belowlight);

//cam positioning
camera.position.set( 2, 2, 2 );
controls.update();

//functions
const loader = new GLTFLoader();
let mixer;

loader.load( 'realthing.glb', function ( gltf ) {
	const model = gltf.scene;
	scene.add( model );
	mixer = new THREE.AnimationMixer(model);
	const clips = gltf.animations;
	clips.forEach(function(clip) {
		const action = mixer.clipAction(clip);
		action.play();
	});

}, undefined, function ( error ) {

	console.error( error );

} );

const clock = new THREE.Clock();

function animate() {
	requestAnimationFrame( animate );
	controls.update();
	if(mixer)
		mixer.update(clock.getDelta());
	renderer.render( scene, camera );
}
animate();
import { initializeApp } from "https://www.gstatic.com/firebasejs/10.8.1/firebase-app.js";
import {getDatabase, ref, onValue } from 'https://www.gstatic.com/firebasejs/10.8.1/firebase-database.js';

const firebaseConfig = {
    apiKey: "AIzaSyC41z5UgfqDsD5ehmk9ouIxQj1DHagjF68",
    authDomain: "dronesimproject2024.firebaseapp.com",
    databaseURL: "https://dronesimproject2024-default-rtdb.europe-west1.firebasedatabase.app",
    projectId: "dronesimproject2024",
    storageBucket: "dronesimproject2024.appspot.com",
    messagingSenderId: "764690212750",
    appId: "1:764690212750:web:9a9e0bf6cb2d44835ca6df",
    measurementId: "G-QM3XZDC0LQ"
  };

const app = initializeApp(firebaseConfig);
const db = getDatabase();

const extra = document.getElementById("extra");

const testRef = ref(db, 'test/float');

onValue(testRef, (snapshot) => {
  const data = snapshot.val();
  extra.innerHTML = data;
});


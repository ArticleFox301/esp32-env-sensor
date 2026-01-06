// Firebase configuration - REPLACE WITH YOUR OWN CONFIG
const firebaseConfig = {
  apiKey: "AIzaSyAsMj5v3FFUK5F-YVN6tpvUorFbb_HqRDQ",
  authDomain: "iot-classroom-monitor.firebaseapp.com",
  databaseURL: "https://iot-classroom-monitor-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "iot-classroom-monitor",
  storageBucket: "iot-classroom-monitor.firebasestorage.app",
  messagingSenderId: "379649941792",
  appId: "1:379649941792:web:65a712fe091baa554b7745"
};
// Initialize Firebase
firebase.initializeApp(firebaseConfig);

// Get a reference to the database service
const database = firebase.database();

// Authentication reference
const auth = firebase.auth();
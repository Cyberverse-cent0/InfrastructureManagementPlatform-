"""
Session Manager for Infrastructure Management Platform
Handles user sessions, authentication, and session storage
"""

import math
import threading
import os
import json
import hashlib
import secrets
from datetime import datetime, timedelta
from typing import Dict, Optional, Any
from pathlib import Path
import ctypes                   #imcplent  global c libabar for user in python 


# Configuration
SESSION_TIMEOUT_HOURS = 24
SESSION_FILE = "data/sessions.json"
USERS_DATABASE_FILE = "data/users.db"

# Thread-safe session storage
sessions_lock = threading.Lock()
sessions: Dict[str, Dict[str, Any]] = {}

# User database storage
users_lock = threading.Lock()
users_database: Dict[str, Dict[str, Any]] = {}

def initialize():
    """Initialize the session manager and load data"""
    # Create data directory if it doesn't exist
    data_dir = Path("data")
    data_dir.mkdir(exist_ok=True)
    
    # Load users database
    load_users_database()
    
    # Load existing sessions
    load_sessions()
    
    print("Session manager initialized")

def load_users_database():
    """Load users from database file"""
    global users_database
    
    if os.path.exists(USERS_DATABASE_FILE):
        try:
            with open(USERS_DATABASE_FILE, 'r') as f:
                users_database = json.load(f)
            print(f"Loaded {len(users_database)} users from database")
        except Exception as e:
            print(f"Error loading users database: {e}")
            users_database = {}
    else:
        # Create default admin user
        users_database = {
            "admin": {
                "username": "admin",
                "password_hash": hash_password("admin123"),  # Default password - change in production
                "role": "admin",
                "created_at": datetime.now().isoformat(),
                "active": True
            }
        }
        save_users_database()
        print("Created default admin user (username: admin, password: admin123)")

def save_users_database():
    """Save users to database file"""
    global users_database
    
    try:
        with open(USERS_DATABASE_FILE, 'w') as f:
            json.dump(users_database, f, indent=2)
    except Exception as e:
        print(f"Error saving users database: {e}")

def load_sessions():
    """Load sessions from file"""
    global sessions
    
    if os.path.exists(SESSION_FILE):
        try:
            with open(SESSION_FILE, 'r') as f:
                loaded_sessions = json.load(f)
            
            # Filter out expired sessions
            current_time = datetime.now()
            valid_sessions = {}
            
            for session_token, session_data in loaded_sessions.items():
                expires_at = datetime.fromisoformat(session_data.get('expires_at'))
                if current_time < expires_at:
                    valid_sessions[session_token] = session_data
            
            with sessions_lock:
                sessions = valid_sessions
            
            print(f"Loaded {len(sessions)} valid sessions")
        except Exception as e:
            print(f"Error loading sessions: {e}")
            sessions = {}

def save_sessions():
    """Save sessions to file"""
    global sessions
    
    try:
        with sessions_lock:
            with open(SESSION_FILE, 'w') as f:
                json.dump(sessions, f, indent=2)
    except Exception as e:
        print(f"Error saving sessions: {e}")

def hash_password(password: str) -> str:
    """Hash a password using SHA-256 (use bcrypt in production)"""
    return hashlib.sha256(password.encode()).hexdigest()

def verify_password(password: str, password_hash: str) -> bool:
    """Verify a password against its hash"""
    return hash_password(password) == password_hash

def generate_session_token() -> str:
    """Generate a secure random session token"""
    return secrets.token_urlsafe(32)

def create_session(username: str, user_data: Dict[str, Any]) -> str:
    """Create a new session for a user"""
    session_token = generate_session_token()
    expires_at = datetime.now() + timedelta(hours=SESSION_TIMEOUT_HOURS)
    
    session_data = {
        "user_id": user_data.get("username", username),
        "username": username,
        "role": user_data.get("role", "user"),
        "created_at": datetime.now().isoformat(),
        "expires_at": expires_at.isoformat(),
        "last_activity": datetime.now().isoformat()
    }
    
    with sessions_lock:
        sessions[session_token] = session_data
    
    save_sessions()
    return session_token

def get_session(session_token: str) -> Optional[Dict[str, Any]]:
    """Get session data by token"""
    with sessions_lock:
        session_data = sessions.get(session_token)
        
        if not session_data:
            return None
        
        # Check if session is expired
        expires_at = datetime.fromisoformat(session_data.get('expires_at'))
        if datetime.now() >= expires_at:
            del sessions[session_token]
            save_sessions()
            return None
        
        # Update last activity
        session_data['last_activity'] = datetime.now().isoformat()
        sessions[session_token] = session_data
        save_sessions()
        
        return session_data

def destroy_session(session_token: str) -> bool:
    """Destroy a session"""
    with sessions_lock:
        if session_token in sessions:
            del sessions[session_token]
            save_sessions()
            return True
        return False

def authenticate_user(username: str, password: str) -> Optional[Dict[str, Any]]:
    """Authenticate a user with username and password"""
    with users_lock:
        user_data = users_database.get(username)
        
        if not user_data:
            return None
        
        if not user_data.get('active', True):
            return None
        
        if verify_password(password, user_data.get('password_hash', '')):
            return user_data
        
        return None

def create_user(username: str, password: str, role: str = "user") -> bool:
    """Create a new user"""
    with users_lock:
        if username in users_database:
            return False
        
        users_database[username] = {
            "username": username,
            "password_hash": hash_password(password),
            "role": role,
            "created_at": datetime.now().isoformat(),
            "active": True
        }
        
        save_users_database()
        return True

def update_user_role(username: str, new_role: str) -> bool:
    """Update user role"""
    with users_lock:
        if username not in users_database:
            return False
        
        users_database[username]['role'] = new_role
        save_users_database()
        return True

def deactivate_user(username: str) -> bool:
    """Deactivate a user"""
    with users_lock:
        if username not in users_database:
            return False
        
        users_database[username]['active'] = False
        save_users_database()
        return True

def cleanup_expired_sessions():
    """Clean up expired sessions"""
    current_time = datetime.now()
    expired_tokens = []
    
    with sessions_lock:
        for session_token, session_data in sessions.items():
            expires_at = datetime.fromisoformat(session_data.get('expires_at'))
            if current_time >= expires_at:
                expired_tokens.append(session_token)
        
        for token in expired_tokens:
            del sessions[token]
        
        if expired_tokens:
            save_sessions()
    
    return len(expired_tokens)

def get_session_stats() -> Dict[str, Any]:
    """Get session statistics"""
    with sessions_lock:
        total_sessions = len(sessions)
        
        # Count active sessions (not expired)
        current_time = datetime.now()
        active_sessions = 0
        
        for session_data in sessions.values():
            expires_at = datetime.fromisoformat(session_data.get('expires_at'))
            if current_time < expires_at:
                active_sessions += 1
        
        return {
            "total_sessions": total_sessions,
            "active_sessions": active_sessions,
            "expired_sessions": total_sessions - active_sessions
        }

def get_user_stats() -> Dict[str, Any]:
    """Get user statistics"""
    with users_lock:
        total_users = len(users_database)
        active_users = sum(1 for user in users_database.values() if user.get('active', True))
        
        # Count by role
        role_counts = {}
        for user in users_database.values():
            role = user.get('role', 'user')
            role_counts[role] = role_counts.get(role, 0) + 1
        
        return {
            "total_users": total_users,
            "active_users": active_users,
            "inactive_users": total_users - active_users,
            "role_distribution": role_counts
        }

# Background cleanup thread
def cleanup_worker():
    """Background thread for cleaning up expired sessions"""
    while True:
        try:
            cleaned = cleanup_expired_sessions()
            if cleaned > 0:
                print(f"Cleaned up {cleaned} expired sessions")
        except Exception as e:
            print(f"Error in cleanup worker: {e}")
        
        # Sleep for 1 hour
        threading.Event().wait(3600)

def start_cleanup_worker():
    """Start the background cleanup worker"""
    cleanup_thread = threading.Thread(target=cleanup_worker, daemon=True)
    cleanup_thread.start()
    print("Session cleanup worker started")

# Initialize on module import
if __name__ != "__main__":
    initialize()
    start_cleanup_worker()
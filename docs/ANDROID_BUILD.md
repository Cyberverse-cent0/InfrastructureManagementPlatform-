# Android App Build Guide

This guide covers building and running the Android application for the Infrastructure Management Platform.

## Prerequisites

- Node.js 18+
- pnpm package manager
- Android Studio (for Android development)
- Expo CLI (for React Native development)

## Installation

### System Dependencies

```bash
# Install system dependencies
make install_deps
```

### Android App Dependencies

```bash
# Install Android app dependencies
make android_install
```

## Build Commands

### Android App

```bash
# Build Android app
make android_build

# Build all frontend components (web + android)
make frontend_android
```

### Development Mode

```bash
# Run Android app in development mode
make android_run

# Serve Android app
make android_serve
```

### Type Checking

```bash
# Type check Android app
make android_typecheck
```

### Cleaning

```bash
# Clean Android app build artifacts
make android_clean
```

## Project Structure

```
frontend/android_app/
├── artifacts/
│   ├── infrastructure-mobile/    # Main React Native app
│   ├── api-server/              # API server integration
│   └── mockup-sandbox/          # UI mockups
├── lib/                         # Shared libraries
├── scripts/                     # Build scripts
├── package.json                 # Workspace configuration
└── pnpm-workspace.yaml         # PNPM workspace configuration
```

## Development Workflow

1. **Install Dependencies**
   ```bash
   make android_install
   ```

2. **Development Mode**
   ```bash
   make android_run
   ```

3. **Build for Production**
   ```bash
   make android_build
   ```

4. **Type Checking**
   ```bash
   make android_typecheck
   ```

## Workspace Configuration

The Android app uses a PNPM workspace with multiple packages:

- `@workspace/infrastructure-mobile` - Main mobile app
- `@workspace/api-client-react` - API client library
- Shared libraries in `lib/` directory

## Environment Variables

The app uses environment variables for configuration:

- `EXPO_PUBLIC_DOMAIN` - Domain for the app
- `EXPO_PUBLIC_REPL_ID` - Replit ID (if using Replit)
- `REPLIT_DEV_DOMAIN` - Replit development domain

## Troubleshooting

### Dependency Issues

If you encounter dependency issues:

```bash
# Clean and reinstall
make android_clean
make android_install
```

### Build Errors

For build errors, check:

1. Node.js version (must be 18+)
2. pnpm version (use latest)
3. Expo CLI installation

### Type Checking Errors

```bash
# Run type check to see errors
make android_typecheck
```

## Integration with Platform

The Android app integrates with the platform via:

1. **API Gateway** - Communicates through the central API gateway
2. **Service Discovery** - Uses dynamic service discovery
3. **Configuration** - Loads configuration from platform config

See `docs/DEPLOYMENT.md` for platform deployment configuration.

## Additional Resources

- [Expo Documentation](https://docs.expo.dev/)
- [React Native Documentation](https://reactnative.dev/)
- [PNPM Workspace Documentation](https://pnpm.io/workspaces)
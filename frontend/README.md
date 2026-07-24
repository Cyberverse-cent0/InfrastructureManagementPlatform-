# Infrastructure Management Platform - Frontend

A feature-rich, type-safe inventory management system built with Next.js, TypeScript, and Tailwind CSS.

## 🚀 Tech Stack

- **Framework**: Next.js 14 (App Router)
- **Language**: TypeScript 5.x
- **Styling**: Tailwind CSS
- **State Management**: Zustand
- **Data Fetching**: React Query (@tanstack/react-query)
- **Forms**: React Hook Form + Zod validation
- **Icons**: Lucide React
- **Charts**: Recharts (ready to implement)

## 📋 Prerequisites

- Node.js 18+ 
- npm or yarn package manager

## 🛠️ Installation

1. Navigate to the frontend directory:
```bash
cd frontend
```

2. Install dependencies:
```bash
npm install
```

## 🏃 Running the Application

### Development Mode
```bash
npm run dev
```
The application will be available at `http://localhost:3000`

### Production Build
```bash
npm run build
npm start
```

### Type Checking
```bash
npm run type-check
```

### Linting
```bash
npm run lint
```

## 📁 Project Structure

```
frontend/
├── src/
│   ├── app/                    # Next.js App Router pages
│   │   ├── layout.tsx         # Root layout
│   │   ├── page.tsx           # Dashboard page
│   │   ├── inventory/         # Inventory management
│   │   ├── analytics/         # Analytics page
│   │   └── globals.css        # Global styles
│   ├── components/            # React components
│   │   ├── ui/               # Reusable UI components
│   │   ├── layout/           # Layout components
│   │   ├── dashboard/        # Dashboard components
│   │   ├── inventory/        # Inventory components
│   │   └── charts/           # Chart components
│   ├── lib/                  # Utility functions
│   │   ├── api.ts           # API client with mock data
│   │   └── utils.ts         # General utilities
│   ├── hooks/               # Custom React hooks
│   │   ├── useAssets.ts     # Asset-related hooks
│   │   └── useDashboard.ts  # Dashboard hooks
│   ├── store/               # State management
│   │   └── globalStore.ts   # Zustand store
│   └── types/               # TypeScript types
│       └── index.ts         # Type definitions
├── public/                  # Static assets
├── package.json            # Dependencies
├── tsconfig.json           # TypeScript config
├── tailwind.config.ts      # Tailwind config
└── next.config.js          # Next.js config
```

## 🎨 Features

### Dashboard
- Real-time statistics overview
- Resource usage visualization
- Recent activity feed
- Asset distribution summary
- Performance metrics

### Inventory Management
- Comprehensive asset listing
- Advanced filtering and search
- Multi-asset selection
- Bulk operations
- CRUD operations
- Status and priority indicators
- Responsive data table

### Analytics
- Time-series data visualization
- Performance metrics tracking
- Resource usage trends
- Detailed metrics table
- Customizable time ranges

## 🔧 Configuration

### API Integration
The current implementation uses mock data in `src/lib/api.ts`. To connect to your backend:

1. Replace the mock functions in `src/lib/api.ts` with actual API calls
2. Update the base URL for your backend API
3. Implement proper error handling and authentication

### Theme Customization
Modify the color scheme in `tailwind.config.ts`:
```typescript
theme: {
  extend: {
    colors: {
      // Add your custom colors
    }
  }
}
```

## 📱 Responsive Design

The application is fully responsive and works on:
- Desktop (1024px+)
- Tablet (640px - 1024px)
- Mobile (< 640px)

## 🔐 Type Safety

The entire application is built with TypeScript for maximum type safety:
- Strongly typed components
- Type-safe API calls
- Type-safe state management
- Compile-time error detection

## 🚀 Performance Optimizations

- Automatic code splitting
- Image optimization
- Static generation where applicable
- Efficient data fetching with React Query
- Optimistic UI updates

## 🧪 Testing

To add testing:
```bash
npm install --save-dev jest @testing-library/react @testing-library/jest-dom
```

## 📝 Next Steps

1. **Backend Integration**: Replace mock API with real backend calls
2. **Authentication**: Implement user authentication
3. **Real-time Updates**: Add WebSocket support for live updates
4. **Chart Implementation**: Integrate Recharts for data visualization
5. **Form Validation**: Complete form handling with Zod schemas
6. **Error Handling**: Implement comprehensive error boundaries
7. **Testing**: Add unit and integration tests
8. **PWA**: Add Progressive Web App capabilities

## 🤝 Contributing

1. Create a feature branch
2. Make your changes
3. Run type checking and linting
4. Submit a pull request

## 📄 License

This project is part of the Infrastructure Management Platform.

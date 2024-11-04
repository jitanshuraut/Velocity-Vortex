
export interface SubMenuItem {
  name: string;
  pathName: string;
  path: string;
  icon?: string;
  active?: boolean;
  items?: SubMenuItem[];
}

export interface MenuItem {
  id: string;
  subheader: string;
  path?: string;
  icon?: string;
  avatar?: string;
  active?: boolean;
  items?: SubMenuItem[];
}

const sitemap: MenuItem[] = [
  {
    id: 'dashboard',
    subheader: 'Dashboard',
    path: '/',
    icon: 'ri:dashboard-fill',
    active: true,
  },
  {
    id: 'Logs',
    subheader: 'Logs',
    path: '/logs',
    icon: 'ic:baseline-show-chart',
    active: false,
  },
  {
    id: 'Portfolio',
    subheader: 'Portfolio',
    path: '/Portfolio',
    icon: 'lineicons:bar-chart-dollar',
    active: false,
  },




];

const updateActiveState = (id: string) => {
  sitemap.forEach((item) => {
    item.active = item.id === id;
  });
};

export { updateActiveState };

export default sitemap;

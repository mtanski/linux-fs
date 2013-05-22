/*
 * Ceph cache definitions.
 *
 *  Copyright (C) 2013 by Adfin Solutions, Inc. All Rights Reserved.
 *  Written by Milosz Tanski (milosz@adfin.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to:
 *  Free Software Foundation
 *  51 Franklin Street, Fifth Floor
 *  Boston, MA  02111-1301  USA
 *
 */

#ifndef _CEPH_CACHE_H
#define _CEPH_CACHE_H
#ifdef CONFIG_CEPH_FSCACHE

#include <linux/fscache.h>


extern struct fscache_netfs ceph_cache_netfs;


void ceph_fscache_inode_get_cookie(struct inode *inode);

void ceph_fscache_register_fsid_cookie(struct ceph_fs_client* fsc);
void ceph_fscache_unregister_fsid_cookie(struct ceph_fs_client* fsc);
void ceph_fscache_register_inode_cookie(struct ceph_fs_client* parent_fsc,
					struct ceph_inode_info* ci);
void ceph_fscache_unregister_inode_cookie(struct ceph_inode_info* ci);
void ceph_fscache_revoke_inode_cookie(struct ceph_inode_info* ci);
void __ceph_fsache_async_uncache_inode(struct ceph_inode_info* ci);

int __ceph_readpage_from_fscache(struct inode *inode, struct page *page);
int __ceph_readpages_from_fscache(struct inode *inode,
				  struct address_space *mapping,
				  struct list_head *pages,
				  unsigned *nr_pages);
void __ceph_readpage_to_fscache(struct inode *inode, struct page *page);
void __ceph_invalidate_fscache_page(struct inode* inode, struct page *page);
int __ceph_release_fscache_page(struct page *page, gfp_t gfp);

static inline void ceph_fsache_async_uncache_inode(struct inode* inode)
{
	struct ceph_inode_info *ci = ceph_inode(inode);

	if (ci->fscache == NULL)
		return;

	__ceph_fsache_async_uncache_inode(ci);
}

static inline int ceph_readpage_from_fscache(struct inode *inode,
					     struct page *page)
{
	if (ceph_inode(inode)->fscache == NULL)
		return -ENOBUFS;

	return __ceph_readpage_from_fscache(inode, page);
}

static inline int ceph_readpages_from_fscache(struct inode *inode,
					      struct address_space *mapping,
					      struct list_head *pages,
					      unsigned *nr_pages)
{
	if (ceph_inode(inode)->fscache == NULL)
		return -ENOBUFS;

	return __ceph_readpages_from_fscache(inode, mapping, pages, nr_pages);
}

static inline void ceph_readpage_to_fscache(struct inode *inode,
					    struct page *page)
{
	if (ceph_inode(inode)->fscache == NULL)
		return;

	if (PageFsCache(page))
		return __ceph_readpage_to_fscache(inode, page);
}

static inline void ceph_invalidate_fscache_page(struct inode *inode,
						struct page *page)
{
	if (ceph_inode(inode)->fscache == NULL)
		return;

	if (PageFsCache(page))
		return __ceph_invalidate_fscache_page(inode, page);
}

static inline int ceph_release_fscache_page(struct page *page, gfp_t gfp)
{
	struct inode* inode = page->mapping->host;
	struct ceph_inode_info *ci = ceph_inode(inode);

	if (ci->fscache == NULL)
		return 1;
	
	return __ceph_release_fscache_page(page, gfp);
}

#endif
#endif
